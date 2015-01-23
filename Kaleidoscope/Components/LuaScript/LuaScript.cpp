#include <Components/LuaScript/LuaScript.h>

#include <GameObject/GameObject.h>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;

#include <Synchronization/Locks/Mutex/Mutex.h>
#include <Synchronization/Locks/Semaphore/Semaphore.h>

#include <string>
#include <algorithm>

#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/format.hpp>
#include <Utility/Parsing/Lowerize.h>

#include <LuaLibs/Utility/lua_old_typenames.h>
#include <LuaLibs/Utility/lua_getters.h>

#include <LuaLibs/Logging/LoggingLibLua.h>
#include <LuaLibs/StringID/StringsLibLua.h>
#include <LuaLibs/Input/InputLibLua.h>
#include <LuaLibs/Application/ApplicationLibLua.h>
#include <LuaLibs/Event/EventLibLua.h>
#include <LuaLibs/Vec3/Vec3LibLua.h>
#include <LuaLibs/Vec4/Vec4LibLua.h>
#include <LuaLibs/Mat4/Mat4LibLua.h>
#include <LuaLibs/Quat/QuatLibLua.h>
#include <LuaLibs/Transform/TransformHandleLibLua.h>
#include <LuaLibs/LuaScript/LUAScriptHandleLibLua.h>
#include <LuaLibs/GameObject/GameObjectHandleLibLua.h>
#include <LuaLibs/Renderable/RenderableHandleLibLua.h>
#include <LuaLibs/Camera/CameraHandleLibLua.h>
#include <LuaLibs/Renderer/RendererLibLua.h>
#include <LuaLibs/Light/LightHandleLibLua.h>

#include <Utility/Parsing/parseMathsFromStrings.h>
#include <Utility/Parsing/generateStringFromMaths.h>
#include <boost/algorithm/string.hpp>

namespace kaleidoscope
{
	static Semaphore createSem;
	static Semaphore nameSem;
	static Semaphore bucketSem;
	static Semaphore errorSem;
	static std::vector< StringID > reservedWorldList;

	LuaScript::LuaScript()
	{
		mInitialized = false;
		mNextInFreeList = NULL;
	}


	/*
	* bool kaleidoscope::LuaScript::init(const kaleidoscope::StringID name, const kaleidoscope::StringID fileName)
	*
	* In: StringID : The internal unique name for the luascript.
	* In: StringID : The path to the .lua file to use.
	* Out: bool : true on success.
	*			  false on failure.
	*
	* Used internally by LuaScript::Create() to initialize a pool object.
	*/
	bool LuaScript::init(const StringID name, const StringID fileName)
	{
		mInitialized = true;
		mName = name;
		mPoolIndex = this - &sLUAScriptPool[0];
		mBucket = 0;
		mEnabled = false;
		mCurrentState = STARTUP_STATE;

		mFileName = fileName;
		mL = luaL_newstate();
		if (mL == NULL)
		{
			setError(CODE_LUA_ERROR, "Could not initialize new Lua state.");
			return false;
		}

		luaL_openlibs(mL);
		kaleidoscope::luaopen_klogging(mL);
		kaleidoscope::luaopen_stringID(mL);
		kaleidoscope::luaopen_kInput(mL);
		kaleidoscope::luaopen_kApplication(mL);
		kaleidoscope::luaopen_vec3(mL);
		kaleidoscope::luaopen_vec4(mL);
		kaleidoscope::luaopen_mat4(mL);
		kaleidoscope::luaopen_quat(mL);
		kaleidoscope::luaopen_event(mL);
		kaleidoscope::luaopen_TransformHandle(mL);
		kaleidoscope::luaopen_LUAScriptHandle(mL);
		kaleidoscope::luaopen_GameObjectHandle(mL);
		kaleidoscope::luaopen_RenderableHandle(mL);
		kaleidoscope::luaopen_CameraHandle(mL);
		kaleidoscope::luaopen_kRenderer(mL);
		kaleidoscope::luaopen_LightHandle(mL);

		if (luaL_dofile(mL, getString(mFileName)))
		{
			gLogManager.log("error loading component %s: %s\n", getString(mFileName), luaL_checklstring(mL, -1, NULL));
			setError(CODE_LUA_ERROR, luaL_checklstring(mL, -1, NULL));
			return false;
		}

		return true;
	}


	/*
	* bool kaleidoscope::LuaScript::destroy()
	* 
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used internally by LuaScript::Destroy() to reset a pool objects state so it may be reused.
	*/
	bool LuaScript::destroy()
	{
		if (mL != NULL)
		{
			lua_close(mL);
		}

		mInitialized = false;
		mNextInFreeList = NULL;

		return true;
	}


	LuaScript::~LuaScript(){}


	/*
	* void kaleidoscope::LuaScript::SerializeIn(const boost::property_tree::ptree& LUAScriptInfo)
	*
	* In: ptree : property tree representing the luascripts internal state.
	* Out: void :
	*
	* Fill in the luascripts state with the values provided by the property tree.
	*
	* <filename>path/to/lua file</filename>
	* <variable>
	*	<name> variable name </name>
	*	<type> variable type </type>
	*	<value> variable value </value>
	* </variable>
	*/
	static const StringID variableID = hashCRC32("variable");
	static const StringID numberID = hashCRC32("number");
	static const StringID boolID = hashCRC32("bool");
	static const StringID stringidID = hashCRC32("stringid");
	static const StringID vec3ID = hashCRC32("vec3");
	static const StringID vec4ID = hashCRC32("vec4");
	static const StringID mat4ID = hashCRC32("mat4");
	static const StringID quatID = hashCRC32("quat");
	static const StringID gameobjecthandleID = hashCRC32("gameobjecthandle");
	void LuaScript::SerializeIn(const boost::property_tree::ptree& LUAScriptInfo)
	{
		using boost::property_tree::ptree;

		BOOST_FOREACH(ptree::value_type const & lsField, LUAScriptInfo)
		{
			StringID lsFieldID = lowerize(lsField.first.c_str());
			if (lsFieldID == variableID)
			{

				ptree const & variableInfo = lsField.second;
				boost::optional<std::string> name = variableInfo.get_optional<std::string>("name");
				boost::optional<std::string> type = variableInfo.get_optional<std::string>("type");
				boost::optional<std::string> value = variableInfo.get_optional<std::string>("value");

				if (name && type && value)
				{
					StringID tID = internString(type->c_str());

					if (tID == numberID)
					{
						setGlobalNumber(name->c_str(), std::stof(*value));
					}
					else if (tID == boolID)
					{
						std::string val = boost::to_lower_copy(*value);
						if (val.compare("true"))
						{
							setGlobalBool(name->c_str(), true);
						}
						else
						{
							setGlobalBool(name->c_str(), false);
						}
					}
					else if (tID == stringidID)
					{
						setGlobalStringID(name->c_str(), static_cast<StringID>(std::stof(*value)));
					}
					else if (tID == vec3ID)
					{
						bool b;
						setGlobalvec3(name->c_str(), stringToVec3(value->c_str(), b));
					}
					else if (tID == vec4ID)
					{
						bool b;
						setGlobalvec4(name->c_str(), stringToVec4(value->c_str(), b));
					}
					else if (tID == mat4ID)
					{
						bool b;
						setGlobalmat4(name->c_str(), stringToMat4(value->c_str(), b));
					}
					else if (tID == quatID)
					{
						bool b;
						setGlobalquat(name->c_str(), stringToQuat(value->c_str(), b));
					}
				}
			}
		}
	}


	/*
	* static kaleidoscope::StringID kaleidoscope::gettypename(lua_State* L, I32 index)
	*
	* In: lua_State* : The lua stack to query.
	* In: I32 : The stack position to query.
	* Out: StringID : The type of the variable at the queried stack position.
	*
	* Used by SerializeOut.
	*/
	static StringID gettypename(lua_State* L, I32 index)
	{
		StringID type = 0;
		I32 vtype = lua_type(L, index);

		if (vtype == LUA_TNUMBER)
		{
			type = internString("number");
		}
		else if (vtype == LUA_TBOOLEAN)
		{
			type = internString("bool");
		}
		else if (vtype == LUA_TSTRING)
		{
			type = internString("string");
		}
		else if (vtype == LUA_TUSERDATA)
		{
			if (luaL_testudata(L, index, "kaleidoscope.vec3") != NULL)
			{
				type = internString("vec3");
			}
			else if (luaL_testudata(L, index, "kaleidoscope.vec4") != NULL)
			{
				type = internString("vec4");
			}
			else if (luaL_testudata(L, index, "kaleidoscope.quat") != NULL)
			{
				type = internString("quat");
			}
			else if (luaL_testudata(L, index, "kaleidoscope.mat4") != NULL)
			{
				type = internString("mat4");
			}
			else if (luaL_testudata(L, index, "kaleidoscope.TransformHandle") != NULL)
			{
				type = internString("TransformHandle");
			}
			else if (luaL_testudata(L, index, "kaleidoscope.GameObjectHandle") != NULL)
			{
				type = internString("GameObjectHandle");
			}
			else if (luaL_testudata(L, index, "kaleidoscope.LUAScriptHande") != NULL)
			{
				type = internString("LUAScriptHandle");
			}
		}

		return type;
	}


	/*
	* boost::property_tree::ptree* kaleidoscope::LuaScript::SerializeOut()
	*
	* In: void :
	* Out: ptree* : a pointer to a ptree representing the internal state of the luascript.
	*
	* THE USER IS RESPONSIBILE FOR DELETING THE POINTER.
	*/
	boost::property_tree::ptree* LuaScript::SerializeOut()
	{
		using boost::property_tree::ptree;

		ptree* lsI = new ptree();
		lsI->add("filename", getString(getFileName()));

		lua_getglobal(mL, "_G");
		I32 t = lua_absindex(mL, -1);
		lua_pushnil(mL);  // first key
		while (lua_next(mL, t) != 0) {

			// uses 'key' (at index -2) and 'value' (at index -1)
			if (lua_type(mL, -2) == LUA_TSTRING && std::find(reservedWorldList.begin(), reservedWorldList.end(), internString(lua_tostring(mL, -2))) == reservedWorldList.end())
			{
				// serialize this var out
				StringID typenm = gettypename(mL, -1);
				if (typenm != 0)
				{
					ptree variableInfo;

					variableInfo.add("name", lua_tostring(mL, -2));
					variableInfo.add("type", getString(typenm));

					if ((typenm == boolID) || (typenm == numberID) || (typenm == internString("string")))
					{
						variableInfo.add("value", lua_tostring(mL, -1));
					}
					else if (typenm == vec3ID)
					{
						math::vec3* v = static_cast<math::vec3*>(luaL_checkudata(mL, -1, "kaleidoscope.vec3"));
						variableInfo.add("value", vec3ToString(*v));
					}
					else if (typenm == vec4ID)
					{
						math::vec4* v = static_cast<math::vec4*>(luaL_checkudata(mL, -1, "kaleidoscope.vec4"));
						variableInfo.add("value", vec4ToString(*v));
					}
					else if (typenm == quatID)
					{
						math::quat* v = static_cast<math::quat*>(luaL_checkudata(mL, -1, "kaleidoscope.quat"));
						variableInfo.add("value", quatToString(*v));
					}
					else if (typenm == mat4ID)
					{
						math::mat4* v = static_cast<math::mat4*>(luaL_checkudata(mL, -1, "kaleidoscope.mat4"));
						variableInfo.add("value", mat4ToString(*v));
					}

					lsI->add_child("variable", variableInfo);
				}
			}
			lua_pop(mL, 1);
		}

		
		return lsI;
	}


	/*
	* kaleidoscope::StringID kaleidoscope::LuaScript::getName() const
	*
	* In: void :
	* Out: StringID : The unique name of this luascript.
	*/
	StringID LuaScript::getName() const
	{
		return mName;
	}


	/*
	* kaleidoscope::StringID kaleidoscope::LuaScript::getFileName() const
	*
	* In: void :
	* Out: StringID : The path to the .lua file this luascript component has been tasked to run.
	*/
	StringID LuaScript::getFileName() const
	{
		return mFileName;
	}


	/*
	* bool kaleidoscope::LuaScript::enabled() const
	*
	* In: void :
	* Out: bool : true if the script will have update() called on it this frame.
	*			  false if the script will not have update() called on it this frame.
	*/
	bool LuaScript::enabled() const
	{
		return mEnabled;
	}


	/*
	* void kaleidoscope::LuaScript::enable()
	*
	* In: void :
	* Out: void :
	*
	* Add this script to the list of scripts to be updated every frame.
	*/
	void LuaScript::enable()
	{
		if (!enabled())
		{
			mEnabled = true;
			if (mCurrentState == STARTUP_STATE)
			{
				sStartupBuckets[mBucket].push_back(LuaScriptHandle(this));
			}
			else if (mCurrentState == UPDATING_STATE)
			{
				sUpdateBuckets[mBucket].push_back(LuaScriptHandle(this));
			}
		}
	}


	/*
	* void kaleidoscope::LuaScript::disable()
	*
	* In: void :
	* Out: void :
	*
	* Remove the script from the list of scripts to be updated every frame.
	*/
	void LuaScript::disable()
	{
		if (enabled())
		{
			mEnabled = false;

			if (mCurrentState == STARTUP_STATE)
			{
				RemoveFromBucket(mBucket, LuaScriptHandle(this), sStartupBuckets);
			}
			else if (mCurrentState == UPDATING_STATE)
			{
				RemoveFromBucket(mBucket, LuaScriptHandle(this), sUpdateBuckets);
			}
		}
	}


	/*
	* void kaleidoscope::LuaScript::startUp()
	*
	* In: void :
	* Out: void :
	*
	* Calls the start function in the provided script the first frame the script is updated.
	*/
	void LuaScript::startUp()
	{
		lua_getglobal(mL, "start");
		if (lua_type(mL, -1) != LUA_TFUNCTION)
		{
			return;
		}
		if (lua_pcall(mL, 0, 0, 0) != 0)
		{
			gLogManager.log("pcall error: start Function: %s", luaL_checklstring(mL, -1, NULL));
		}
	}


	/*
	* void kaleidoscope::LuaScript::update(F32 dt)
	*
	* In: F32 : The time since update was last called on this script.
	* Out: void :
	*
	* calls the update( number ) function in the provided script.
	*/
	void LuaScript::update(F32 dt)
	{
		lua_getglobal(mL, "update");
		if (lua_type(mL, -1) != LUA_TFUNCTION)
		{
			lua_pop(mL, 1);
			return;
		}
		lua_pushnumber(mL, dt);
		if (lua_pcall(mL, 1, 0, 0) != 0)
		{
			gLogManager.log("pcall error: update Function: %s", luaL_checklstring(mL, -1, NULL) );
			lua_pop(mL, 1);
		}
	}


	/*
	* void kaleidoscope::LuaScript::shutdown()
	*
	* In: void :
	* Out: void :
	*
	* Not currently implemented.
	*/
	void LuaScript::shutdown()
	{
	}


	/*
	* void kaleidoscope::LuaScript::onEvent(const kaleidoscope::Event& e)
	*
	* In: Event : The event to feed to the script.
	* Out: void :
	*
	* This function looks to see if the script has a function with the provided events type name
	*	and if it does, it calls that function and passes it the event structure.
	*/
	void LuaScript::onEvent(const Event& e)
	{
		lua_getglobal(mL, getString(e.getEventType()));
		if (lua_type(mL, -1) != LUA_TFUNCTION)
		{
			lua_pop(mL, 1);
			return;
		}
		else
		{
			kaleidoscope::Event* lua_e = newudata<Event>(mL, eventTypeName);
			*lua_e = e;
			if (lua_pcall(mL, 1, 0, 0) != 0)
			{
				gLogManager.log("pcall error: onEvent Function");
				lua_pop(mL, 1);
			}
		}
	}


	/*
	* F32 kaleidoscope::LuaScript::getGlobalNumber(const char * name, bool& success)
	*
	* In: const char * : The name of the variable to get
	* In: bool : true if the variable exists and is of the correct type.
	*			 false if the variable doesn't exist or is of the incorrect type.
	* Out: F32 : the value of the variable
	*/
	// T getT(StringID name)
	F32 LuaScript::getGlobalNumber(const char * name, bool& success)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TNUMBER)
		{
			success = true;
			return static_cast<F32>(lua_tonumber(mL, -1));
		} 
		else
		{
			success = false;
			return 0.0f;
		}
	}


	/*
	* F32 kaleidoscope::LuaScript::getGlobalBool(const char * name, bool& success)
	*
	* In: const char * : The name of the variable to get
	* In: bool : true if the variable exists and is of the correct type.
	*			 false if the variable doesn't exist or is of the incorrect type.
	* Out: bool : the value of the variable
	*/
	bool LuaScript::getGlobalBool(const char * name, bool& success)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TBOOLEAN)
		{
			success = true;
			return (lua_toboolean(mL, -1) != 0);
		}
		else
		{
			success = false;
			return false;
		}
	}


	/*
	* F32 kaleidoscope::LuaScript::getGlobalStringID(const char * name, bool& success)
	*
	* In: const char * : The name of the variable to get
	* In: bool : true if the variable exists and is of the correct type.
	*			 false if the variable doesn't exist or is of the incorrect type.
	* Out: StringID : the value of the variable
	*/
	StringID LuaScript::getGlobalStringID(const char * name, bool& success)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TNUMBER)
		{
			success = true;
			return static_cast<StringID>(luaL_checkunsigned(mL, -1));
		}
		else
		{
			success = false;
			return LuaScript::NAME;
		}
	}


	/*
	* F32 kaleidoscope::LuaScript::getGlobalvec3(const char * name, bool& success)
	*
	* In: const char * : The name of the variable to get
	* In: bool : true if the variable exists and is of the correct type.
	*			 false if the variable doesn't exist or is of the incorrect type.
	* Out: vec3 : the value of the variable
	*/
	math::vec3 LuaScript::getGlobalvec3(const char * name, bool& success)
	{
		lua_getglobal(mL, name);
		if (luaL_testudata(mL, -1, "kaleidoscope.vec3") != NULL)
		{
			success = true;
			return *(static_cast<math::vec3*>(luaL_checkudata(mL, -1, "kaleidoscope.vec3")));
		}
		else
		{
			success = false;
			return math::vec3();
		}
	}


	/*
	* F32 kaleidoscope::LuaScript::getGlobalvec4(const char * name, bool& success)
	*
	* In: const char * : The name of the variable to get
	* In: bool : true if the variable exists and is of the correct type.
	*			 false if the variable doesn't exist or is of the incorrect type.
	* Out: vec4 : the value of the variable
	*/
	math::vec4 LuaScript::getGlobalvec4(const char * name, bool& success)
	{
		lua_getglobal(mL, name);
		if (luaL_testudata(mL, -1, "kaleidoscope.vec4") != NULL)
		{
			success = true;
			return *(static_cast<math::vec4*>(luaL_checkudata(mL, -1, "kaleidoscope.vec4")));
		}
		else
		{
			success = false;
			return math::vec4();
		}
	}


	/*
	* F32 kaleidoscope::LuaScript::getGlobalmat4(const char * name, bool& success)
	*
	* In: const char * : The name of the variable to get
	* In: bool : true if the variable exists and is of the correct type.
	*			 false if the variable doesn't exist or is of the incorrect type.
	* Out: mat4 : the value of the variable
	*/
	math::mat4 LuaScript::getGlobalmat4(const char * name, bool& success)
	{
		lua_getglobal(mL, name);
		if (luaL_testudata(mL, -1, "kaleidoscope.mat4") != NULL)
		{
			success = true;
			return *(static_cast<math::mat4*>(luaL_checkudata(mL, -1, "kaleidoscope.mat4")));
		}
		else
		{
			success = false;
			return math::mat4();
		}
	}


	/*
	* F32 kaleidoscope::LuaScript::getGlobalquat(const char * name, bool& success)
	*
	* In: const char * : The name of the variable to get
	* In: bool : true if the variable exists and is of the correct type.
	*			 false if the variable doesn't exist or is of the incorrect type.
	* Out: quat : the value of the variable
	*/
	math::quat LuaScript::getGlobalquat(const char * name, bool& success)
	{
		lua_getglobal(mL, name);
		if (luaL_testudata(mL, -1, "kaleidoscope.quat") != NULL)
		{
			success = true;
			return *(static_cast<math::quat*>(luaL_checkudata(mL, -1, "kaleidoscope.quat")));
		}
		else
		{
			success = false;
			return math::quat();
		}
	}


	/*
	* F32 kaleidoscope::LuaScript::getGlobalGameObjectHandle(const char * name, bool& success)
	*
	* In: const char * : The name of the variable to get
	* In: bool : true if the variable exists and is of the correct type.
	*			 false if the variable doesn't exist or is of the incorrect type.
	* Out: GameObjectHandle : the value of the variable
	*/
	GameObjectHandle LuaScript::getGlobalGameObjectHandle(const char * name, bool& success)
	{
		lua_getglobal(mL, name);
		if (luaL_testudata(mL, -1, "kaleidoscope.GameObjectHandle") != NULL)
		{
			success = true;
			return *(static_cast<GameObjectHandle*>(luaL_checkudata(mL, -1, "kaleidoscope.GameObjectHandle")));
		}
		else
		{
			success = false;
			return GameObjectHandle::null;
		}
	}


	/*
	* void kaleidoscope::LuaScript::setGlobalNumber(const char * name, F32 value)
	*
	* In: const char * : The name of the variable to get.
	* In: F32 : The new value if the variable.
	* Out: void :
	*
	* Variables value is not set if it is not of the correct type.
	*/
	void LuaScript::setGlobalNumber(const char * name, F32 value)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TNUMBER)
		{
			lua_pop(mL, 1);
			lua_pushnumber(mL, value);
			lua_setglobal(mL, name);
		}
	}


	/*
	* void kaleidoscope::LuaScript::setGlobalBool(const char * name, F32 value)
	*
	* In: const char * : The name of the variable to get.
	* In: bool : The new value if the variable.
	* Out: void :
	*
	* Variables value is not set if it is not of the correct type.
	*/
	void LuaScript::setGlobalBool(const char * name, bool value)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TBOOLEAN)
		{
			lua_pop(mL, 1);
			lua_pushboolean(mL, value);
			lua_setglobal(mL, name);
		}
	}


	/*
	* void kaleidoscope::LuaScript::setGlobalStringID(const char * name, F32 value)
	*
	* In: const char * : The name of the variable to get.
	* In: StringID : The new value if the variable.
	* Out: void :
	*
	* Variables value is not set if it is not of the correct type.
	*/
	void LuaScript::setGlobalStringID(const char * name, StringID value)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TNUMBER)
		{
			lua_pop(mL, 1);
			lua_pushnumber(mL, value);
			lua_setglobal(mL, name);
		}
	}


	/*
	* void kaleidoscope::LuaScript::setGlobalvec3(const char * name, F32 value)
	*
	* In: const char * : The name of the variable to get.
	* In: vec3 : The new value if the variable.
	* Out: void :
	*
	* Variables value is not set if it is not of the correct type.
	*/
	void LuaScript::setGlobalvec3(const char * name, const math::vec3& value)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TUSERDATA && luaL_testudata(mL, -1, "kaleidoscope.vec3") != NULL)
		{
			lua_pop(mL, 1);
			math::vec3* n = static_cast<math::vec3*>(lua_newuserdata(mL, sizeof(math::vec3)));
			luaL_getmetatable(mL, "kaleidoscope.vec3");
			lua_setmetatable(mL, -2);
			lua_setglobal(mL, name);
		}
	}


	/*
	* void kaleidoscope::LuaScript::setGlobalvec4(const char * name, F32 value)
	*
	* In: const char * : The name of the variable to get.
	* In: vec4 : The new value if the variable.
	* Out: void :
	*
	* Variables value is not set if it is not of the correct type.
	*/
	void LuaScript::setGlobalvec4(const char * name, const math::vec4& value)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TUSERDATA && luaL_testudata(mL, -1, "kaleidoscope.vec4") != NULL)
		{
			lua_pop(mL, 1);
			math::vec4* n = static_cast<math::vec4*>(lua_newuserdata(mL, sizeof(math::vec4)));
			luaL_getmetatable(mL, "kaleidoscope.vec4");
			lua_setmetatable(mL, -2);
			lua_setglobal(mL, name);
		}
	}


	/*
	* void kaleidoscope::LuaScript::setGlobalmat4(const char * name, F32 value)
	*
	* In: const char * : The name of the variable to get.
	* In: mat4 : The new value if the variable.
	* Out: void :
	*
	* Variables value is not set if it is not of the correct type.
	*/
	void LuaScript::setGlobalmat4(const char * name, const math::mat4& value)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TUSERDATA && luaL_testudata(mL, -1, "kaleidoscope.mat4") != NULL)
		{
			lua_pop(mL, 1);
			math::mat4* n = static_cast<math::mat4*>(lua_newuserdata(mL, sizeof(math::mat4)));
			luaL_getmetatable(mL, "kaleidoscope.mat4");
			lua_setmetatable(mL, -2);
			lua_setglobal(mL, name);
		}
	}


	/*
	* void kaleidoscope::LuaScript::setGlobalquat(const char * name, F32 value)
	*
	* In: const char * : The name of the variable to get.
	* In: quat : The new value if the variable.
	* Out: void :
	*
	* Variables value is not set if it is not of the correct type.
	*/
	void LuaScript::setGlobalquat(const char * name, const math::quat& value)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TUSERDATA && luaL_testudata(mL, -1, "kaleidoscope.quat") != NULL)
		{
			lua_pop(mL, 1);
			math::quat* n = static_cast<math::quat*>(lua_newuserdata(mL, sizeof(math::quat)));
			luaL_getmetatable(mL, "kaleidoscope.quat");
			lua_setmetatable(mL, -2);
			lua_setglobal(mL, name);
		}
	}


	/*
	* void kaleidoscope::LuaScript::setGlobalGameObjectHandle(const char * name, F32 value)
	*
	* In: const char * : The name of the variable to get.
	* In: GameObjectHandle : The new value if the variable.
	* Out: void :
	*
	* Variables value is not set if it is not of the correct type.
	*/
	void LuaScript::setGlobalGameObjectHandle(const char * name, const GameObjectHandle& value)
	{
		lua_getglobal(mL, name);
		if (lua_type(mL, -1) == LUA_TUSERDATA && luaL_testudata(mL, -1, "kaleidoscope.GameObjectHandle") != NULL)
		{
			lua_pop(mL, 1);
			GameObjectHandle* n = static_cast<GameObjectHandle*>(lua_newuserdata(mL, sizeof(GameObjectHandle)));
			luaL_getmetatable(mL, "kaleidoscope.GameObjectHandle");
			lua_setmetatable(mL, -2);
			lua_setglobal(mL, name);
		}
	}


	/*
	* void kaleidoscope::LuaScript::printState() const
	*
	* In:  void :
	* Out: void :
	*
	* Print the internal state of the luascript
	* NEEDS TO BE REPLACED
	*	replace with CSV
	*/
	void LuaScript::printState() const
	{
		gLogManager.log("		name = %s", getString(mName));
		gLogManager.log("		filename = %s", getString(mFileName));
		gLogManager.log("		mPoolIndex = %i", mPoolIndex);
		gLogManager.log("		mBucket = %u", mBucket);
	}


	/*
	* bool kaleidoscope::LuaScript::StartUp(const boost::property_tree::ptree& properties)
	* 
	* In: ptree : The properties used to start up the luascript subsystem
	* Out: bool : true on success.
	*			  false on failure.
	*
	* objects = U32 The maximum number of luascript that can be allocated.
	* buckets = U32 The maximum number of update buckets.
	*/
	bool LuaScript::StartUp(const boost::property_tree::ptree& properties)
	{
		if (!initialized)
		{

			initialized = true;

			boost::optional<U32> numObjects = properties.get_optional<U32>("objects");
			boost::optional<U32> numBuckets = properties.get_optional<U32>("buckets");

			if (numObjects)
			{
				MAXNUMOBJECTS = *numObjects;
			}
			else
			{
				MAXNUMOBJECTS = DEFAULTMAXOBJS;
			}

			if (numBuckets)
			{
				NUMBUCKETS = *numBuckets;
			}
			else
			{
				NUMBUCKETS = DEFAULTMAXBUCKS;
			}

			if (Semaphore::Semaphore_init(&createSem, 1) != 0 || Semaphore::Semaphore_init(&nameSem, 1) != 0 || Semaphore::Semaphore_init(&bucketSem, 1) != 0 || Semaphore::Semaphore_init(&errorSem, 1) != 0)
			{
				initialized = false;
				return false;
			}

			mErrorManager = ErrorManager();

			sLUAScriptPool = new LuaScript[MAXNUMOBJECTS];

			sFirstFree = &sLUAScriptPool[0];

			for (U32 i = 0; i < MAXNUMOBJECTS - 1; ++i)
			{
				sLUAScriptPool[i].mNextInFreeList = &sLUAScriptPool[i + 1];
			}
			sLUAScriptPool[MAXNUMOBJECTS - 1].mNextInFreeList = NULL;

			sStartupBuckets.reserve(NUMBUCKETS);
			sUpdateBuckets.reserve(NUMBUCKETS);
			for (U32 i = 0; i < NUMBUCKETS; ++i)
			{
				sStartupBuckets.push_back(std::list<LuaScriptHandle>());
				sUpdateBuckets.push_back(std::list<LuaScriptHandle>());
			}


			reservedWorldList.push_back(internString("assert"));
			reservedWorldList.push_back(internString("collectgarbade"));
			reservedWorldList.push_back(internString("dofile"));
			reservedWorldList.push_back(internString("error"));
			reservedWorldList.push_back(internString("getmetatable"));
			reservedWorldList.push_back(internString("ipairs"));
			reservedWorldList.push_back(internString("load"));
			reservedWorldList.push_back(internString("loadfile"));
			reservedWorldList.push_back(internString("next"));
			reservedWorldList.push_back(internString("pairs"));
			reservedWorldList.push_back(internString("pcall"));
			reservedWorldList.push_back(internString("print"));
			reservedWorldList.push_back(internString("rawequal"));
			reservedWorldList.push_back(internString("rawget"));
			reservedWorldList.push_back(internString("rawlen"));
			reservedWorldList.push_back(internString("rawset"));
			reservedWorldList.push_back(internString("require"));
			reservedWorldList.push_back(internString("select"));
			reservedWorldList.push_back(internString("setmetatable"));
			reservedWorldList.push_back(internString("tonumber"));
			reservedWorldList.push_back(internString("tostring"));
			reservedWorldList.push_back(internString("type"));
			reservedWorldList.push_back(internString("xpcall"));
			reservedWorldList.push_back(internString("bit32"));
			reservedWorldList.push_back(internString("coroutine"));
			reservedWorldList.push_back(internString("debug"));
			reservedWorldList.push_back(internString("file"));
			reservedWorldList.push_back(internString("io"));
			reservedWorldList.push_back(internString("math"));
			reservedWorldList.push_back(internString("os"));
			reservedWorldList.push_back(internString("package"));
			reservedWorldList.push_back(internString("string"));
			reservedWorldList.push_back(internString("table"));
			reservedWorldList.push_back(internString("vec3"));
			reservedWorldList.push_back(internString("vec4"));
			reservedWorldList.push_back(internString("TransformHandle"));
			reservedWorldList.push_back(internString("kStringID"));
			reservedWorldList.push_back(internString("quat"));
			reservedWorldList.push_back(internString("mat4"));
			reservedWorldList.push_back(internString("LUAScriptHandle"));
			reservedWorldList.push_back(internString("kLogging"));
			reservedWorldList.push_back(internString("kInput"));
			reservedWorldList.push_back(internString("GameObjectHandle"));
			reservedWorldList.push_back(internString("kEvent"));
			reservedWorldList.push_back(internString("kApplication"));
			reservedWorldList.push_back(internString("_VERSION"));

			return true;

		}

		return true;

	}


	/*
	* bool kaleidoscope::LuaScript::ShutDown()
	*
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used to shutdown the luascirpt subsystem.
	*/
	bool LuaScript::ShutDown()
	{
		if (initialized)
		{
			initialized = false;

			delete[] sLUAScriptPool;

			Semaphore::Semaphore_destroy(&createSem);
			Semaphore::Semaphore_destroy(&nameSem);
			Semaphore::Semaphore_destroy(&bucketSem);

			return true;
		}

		return true;
	}


	/*
	* kaleidoscope::LuaScriptHandle kaleidoscope::LuaScript::Create(const kaleidoscope::StringID fileName)
	*
	* In: StringID : The path to the .lua file to run.
	* Out: LuaScriptHandle :  An freshly initialized renderable.
	*						  LuaScriptHandle::null if the initilization failed.
	*
	* Error handling functions can be used to get the reason the creation failed.
	*/
	LuaScriptHandle LuaScript::Create(const StringID fileName)
	{
		Semaphore::Semaphore_wait(&createSem);

		if (sFirstFree != NULL)
		{
			LuaScript* const newScript = sFirstFree;
			sFirstFree = newScript->mNextInFreeList;
			bool b = newScript->init(GenerateName(), fileName);

			// Handle init failure.
			if (b == false)
			{
				newScript->destroy();

				U32 i = (newScript - &sLUAScriptPool[0]);
				sLUAScriptPool[i].mNextInFreeList = sFirstFree;
				sFirstFree = &sLUAScriptPool[i];

				Semaphore::Semaphore_post(&createSem);
				return LuaScriptHandle::null;
			}

			LuaScriptHandle lh(newScript);
			Semaphore::Semaphore_post(&createSem);
			return lh;
		} 
		else
		{
			setError(CODE_POOL_FULL, "The maximum number of luascripts has been allocated, increase the number of luascripts specified in properties at StartUp");
			Semaphore::Semaphore_post(&createSem);
			return LuaScriptHandle::null;
		}
	}


	/*
	* kaleidoscope::LuaScriptHandle kaleidoscope::LuaScript::Create(const boost::property_tree::ptree& LUAScriptInfo)
	*
	* In: ptree : The properties to initialize the luascript with.
	* Out: LuaScriptHandle : An initialized luascript.
	*						  LuaScriptHandle::null if the initilization failed.
	*
	* Error handling functions can be used to get the reason the creation failed.
	*/
	LuaScriptHandle LuaScript::Create(const boost::property_tree::ptree& LUAScriptInfo)
	{
		StringID fileName = 0;
		// Get file name from ptree;
		using boost::property_tree::ptree;
		boost::optional<std::string> fname = LUAScriptInfo.get_optional<std::string>("filename");
		if (!fname)
		{
			return LuaScriptHandle::null;
		}
		fileName = internString(fname->c_str());

		LuaScriptHandle lh = LuaScript::Create(fileName);
		if (lh != LuaScriptHandle::null)
		{
			lh.getObject()->SerializeIn(LUAScriptInfo);
		}
		return lh;
	}


	/*
	* void kaleidoscope::LuaScript::Destroy(const kaleidoscope::LuaScriptHandle& LUAScriptToDestroy)
	*
	* In: LuaScript : The luascript to destroy.
	* Out: void :
	*/
	void LuaScript::Destroy(const LuaScriptHandle& LUAScriptToDestroy)
	{
		Semaphore::Semaphore_wait(&createSem);

		if (LUAScriptToDestroy.getObject() != NULL)
		{

			U64 i = ((LUAScriptToDestroy.getObject()) - &sLUAScriptPool[0]);
			sLUAScriptPool[i].destroy();
			sLUAScriptPool[i].mNextInFreeList = sFirstFree;
			sFirstFree = &sLUAScriptPool[i];
		}

		Semaphore::Semaphore_post(&createSem);
	}


	/*
	* void kaleidoscope::LuaScript::printBuckets()
	*
	* In: void :
	* Out: void :
	*
	* Print out a representation of which scripts are in which update bucket.
	*/
	void LuaScript::printBuckets()
	{
		gLogManager.log("Startup Buckets.");
		for (U32 i = 0; i < NUMBUCKETS; ++i)
		{
			gLogManager.log("Bucket %u", i);
			for (std::list<LuaScriptHandle>::iterator lh = sStartupBuckets[i].begin(); lh != sStartupBuckets[i].end(); ++lh)
			{
				gLogManager.log("	script (%s, %s)", getString((*lh).getName()), getString((*lh).getFileName()));
			}

			gLogManager.log("");
		}

		gLogManager.log("\n\nUpdate Buckets.");
		for (U32 i = 0; i < NUMBUCKETS; ++i)
		{
			gLogManager.log("Bucket %u", i);
			sUpdateBuckets[i];
			for (std::list<LuaScriptHandle>::iterator lh = sUpdateBuckets[i].begin(); lh != sUpdateBuckets[i].end(); ++lh)
			{
				gLogManager.log("	script (%s, %s)", getString((*lh).getName()), getString((*lh).getFileName()));
			}

			gLogManager.log("");
		}
	}


	/*
	* bool kaleidoscope::LuaScript::hasPendingError()
	*
	* In: void :
	* Out: bool : true if there is an error.
	*			  false if there isnt an error.
	*
	* If Create throws an error the system logs an error. The error state stays the same until it is either cleared by
	*	the user or Create throws another error. In this case the previous error is overwritten.
	*/
	bool LuaScript::hasPendingError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		bool b = mErrorManager.hasPendingError();
		Semaphore::Semaphore_post(&errorSem);
		return b;
	}


	/*
	* void kaleidoscope::LuaScript::clearError()
	*
	* In: void :
	* Out: void :
	*
	* If there was an error in the transform system clear it out.
	* Used after processing an error to tell when the next error occurs.
	*/
	void LuaScript::clearError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.clearError();
		Semaphore::Semaphore_post(&errorSem);
	}


	/*
	* kaleidoscope::ErrorCode kaleidoscope::LuaScript::getErrorCode()
	*
	* In: void :
	* Out: ErrorCode : The error code representing the current state of the transform system.
	*/
	ErrorCode LuaScript::getErrorCode()
	{
		Semaphore::Semaphore_wait(&errorSem);
		ErrorCode ec = mErrorManager.getErrorCode();
		Semaphore::Semaphore_post(&errorSem);
		return ec;
	}


	/*
	* std::string kaleidoscope::LuaScript::getErrorString()
	*
	* In: void :
	* Out: ErrorCode : The detailed string explaining why the error happened.
	*/
	std::string LuaScript::getErrorString()
	{
		Semaphore::Semaphore_wait(&errorSem);
		std::string es = mErrorManager.getErrorString();
		Semaphore::Semaphore_post(&errorSem);
		return es;
	}


	/*
	* void kaleidoscope::LuaScript::setError(kaleidoscope::ErrorCode ec, const std::string& es)
	*
	* In: ErrorCode : What error happened in the system.
	* In: string : The detailed description of what happened.
	* Out: void :
	*
	* used internally.
	*/
	void LuaScript::setError(ErrorCode ec, const std::string& es)
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.setError(ec, es);
		Semaphore::Semaphore_post(&errorSem);
	}



	/*
	* kaleidoscope::StringID kaleidoscope::LuaScript::GenerateName()
	*
	* In: void :
	* Out: StringID : The name generated for the next renderable.
	*
	* Used internally to make sure each renderable has a unique name.
	*/
	U32 LuaScript::numScripts = 0;
	StringID LuaScript::GenerateName()
	{
		char buff[25];
		Semaphore::Semaphore_wait(&nameSem);
		I32 n = sprintf_s(buff, "Script__%u", numScripts++);
		Semaphore::Semaphore_post(&nameSem);
		StringID name = internString(buff);
		return name;
	}


	/*
	* void kaleidoscope::LuaScript::UpdateAll(F32 dt)
	*
	* In: F32 : The time since the last call to UpdateAll()
	* Out: void :
	*
	* Simulates all active luascripts.
	* Calls startup() if they are new, and calls update() on all.
	*/
	void LuaScript::UpdateAll(F32 dt)
	{
		// Startup each script.
		for (U32 bucket = 0; bucket < NUMBUCKETS; ++bucket)
		{
			for (std::list<LuaScriptHandle>::iterator scr = sStartupBuckets[bucket].begin(); scr != sStartupBuckets[bucket].end(); )
			{
				std::list<LuaScriptHandle>::iterator curr = scr++;
				if ((*curr).valid())
				{
					(*curr).getObject()->startUp();
					(*curr).getObject()->mCurrentState = UPDATING_STATE;
					sUpdateBuckets[bucket].push_back((*curr));
				}
				sStartupBuckets[bucket].erase(curr);
			}
		}

		// update each script.
		for (U32 bucket = 0; bucket < NUMBUCKETS; ++bucket)
		{
			for (std::list<LuaScriptHandle>::iterator scr = sUpdateBuckets[bucket].begin(); scr != sUpdateBuckets[bucket].end(); ++scr)
			{
				if ((*scr).valid() && (*scr).getObject()->mBucket == bucket)
				{
					(*scr).getObject()->update(dt);
				}
				else
				{
					std::list<LuaScriptHandle>::iterator curr = scr++;
					sUpdateBuckets[bucket].erase(curr);
				}
			}
		}

	}


	/*
	* void kaleidoscope::LuaScript::AddToBucket(const U32 bucket, const kaleidoscope::LuaScriptHandle& lh)
	*
	* In: U32 : The bucket to add them to.
	* In: LuaScriptHandle : The script to add to the bucket.
	* Out: void : 
	* 
	* Add a script to a certain update bucket.
	*/
	void LuaScript::AddToBucket(const U32 bucket, const LuaScriptHandle& lh)
	{
		if (lh.getObject()->mCurrentState == STARTUP_STATE)
		{
			LuaScript::AddToBucket(bucket, lh, sStartupBuckets);
		}
		else if (lh.getObject()->mCurrentState == UPDATING_STATE)
		{
			LuaScript::AddToBucket(bucket, lh, sUpdateBuckets);
		}
	}


	/*
	* void kaleidoscope::LuaScript::AddToBucket(const U32 bucket, const kaleidoscope::LuaScriptHandle& lh, std::vector< std::list<kaleidoscope::LuaScriptHandle> >& uBuckets)
	*
	* In: U32 : The bucket to add them to.
	* In: LuaScriptHandle : The script to add to the bucket.
	* In: vector< list< LuaScriptHandle> > : The bucket set to add the script to.
	* Out: void :
	*
	* Used internally by AddToBucket() above.
	*/
	void LuaScript::AddToBucket(const U32 bucket, const LuaScriptHandle& lh, std::vector< std::list<LuaScriptHandle> >& uBuckets)
	{
		Semaphore::Semaphore_wait(&bucketSem);
		if (lh.isEnabled())
		{
			uBuckets[bucket].push_back(lh);
		}
		lh.getObject()->mBucket = bucket;
		Semaphore::Semaphore_post(&bucketSem);
	}


	/*
	* void kaleidoscope::LuaScript::RemoveFromBucket(const U32 bucket, const kaleidoscope::LuaScriptHandle& lh, std::vector< std::list<kaleidoscope::LuaScriptHandle> >& uBuckets)
	*
	* In: U32 : The bucket to remove the script from.
	* In: LuaScriptHandle : The script to remove to the bucket.
	* In: vector< list< LuaScriptHandle> > : The bucket set to remove the script from.
	* Out: void :
	*/
	void LuaScript::RemoveFromBucket(const U32 bucket, const LuaScriptHandle& lh, std::vector< std::list<LuaScriptHandle> >& uBuckets)
	{
		if (lh.isEnabled())
		{
			const std::list<LuaScriptHandle>::iterator bucketLocation = std::find(uBuckets[bucket].begin(), uBuckets[bucket].end(), lh);
			if (bucketLocation != uBuckets[bucket].end())
			{
				uBuckets[bucket].erase(bucketLocation);
			}
		}
		lh.getObject()->mBucket = 0;
	}

	U32 LuaScript::MAXNUMOBJECTS;
	U32 LuaScript::NUMBUCKETS;
	StringID LuaScript::NAME = hashCRC32("luascript");

	std::vector< std::list<LuaScriptHandle> > LuaScript::sStartupBuckets;
	std::vector< std::list<LuaScriptHandle> > LuaScript::sUpdateBuckets;

	const U32 LuaScript::DEFAULTMAXOBJS = 10;
	const U32 LuaScript::DEFAULTMAXBUCKS = 3;

	bool LuaScript::initialized = false;

	LuaScript* LuaScript::sFirstFree = NULL;
	LuaScript* LuaScript::sLUAScriptPool = NULL;

	ErrorManager LuaScript::mErrorManager;
}