#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringId/StringId.h>

#include <Debug/ErrorManagement/ErrorCodes.h>
#include <Debug/ErrorManagement/ErrorManager.h>

#include <Components/LuaScript/LuaScriptHandle.h>

#include <list>
#include <vector>

extern "C"
{
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

#include <boost/property_tree/ptree.hpp>


namespace kaleidoscope
{
	class LuaScript
	{
		friend class LuaScriptHandle;


		static U32 NUMBUCKETS;
		static U32 MAXNUMOBJECTS;


		static StringID NAME;

		enum ScriptState{
			STARTUP_STATE,
			UPDATING_STATE,
			ENDING_STATE
		};

		LuaScript();
		bool init(const StringID name, const StringID fileName);
		bool destroy();
		~LuaScript();

		void SerializeIn(const boost::property_tree::ptree& scriptInfo);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const;
		StringID getFileName() const;

		bool enabled() const;
		void enable();
		void disable();

		void startUp();
		void update(F32 dt);
		void shutdown();

		void onEvent(const Event& e);

		// T getT(StringID name)
		F32 getGlobalNumber(const char * name, bool& success);
		bool getGlobalBool(const char * name, bool& success);
		StringID getGlobalStringID(const char * name, bool& success);
		math::vec3 getGlobalvec3(const char * name, bool& success);
		math::vec4 getGlobalvec4(const char * name, bool& success);
		math::mat4 getGlobalmat4(const char * name, bool& success);
		math::quat getGlobalquat(const char * name, bool& success);
		GameObjectHandle getGlobalGameObjectHandle(const char * name, bool& success);

		// void setT(StringID name, T value);
		void setGlobalNumber(const char * name, F32 value);
		void setGlobalBool(const char * name, bool value);
		void setGlobalStringID(const char * name, StringID value);
		void setGlobalvec3(const char * name, const math::vec3& value);
		void setGlobalvec4(const char * name, const math::vec4& value);
		void setGlobalmat4(const char * name, const math::mat4& value);
		void setGlobalquat(const char * name, const math::quat& value);
		void setGlobalGameObjectHandle(const char * name, const GameObjectHandle& value);

		template <class T>
		void addudata(const char * name, const char * udataName, T dataToCopy);

		void printState() const;


		bool mInitialized;
		union
		{
			class
			{
			public:
				StringID mName;
				I16 mPoolIndex;
				U32 mBucket;
				bool mEnabled;
				ScriptState mCurrentState;

				StringID mFileName;
				lua_State* mL;

			};

			LuaScript* mNextInFreeList;
		};


		static bool StartUp(const boost::property_tree::ptree& properties);
		static bool ShutDown();

		static LuaScriptHandle Create(const StringID fileName);
		static LuaScriptHandle Create(const boost::property_tree::ptree& scriptInfo);

		static void Destroy(const LuaScriptHandle& scriptToDestroy);

		static void UpdateAll(F32 dt);

		static void printBuckets();

		static bool hasPendingError();
		static void clearError();
		static ErrorCode getErrorCode();
		static std::string getErrorString();
		static void setError(ErrorCode ec, const std::string& es);




		static U32 numScripts;
		static StringID GenerateName();

		static void AddToBucket(const U32 bucket, const LuaScriptHandle& lh);
		static void AddToBucket(const U32 bucket, const LuaScriptHandle& lh, std::vector< std::list<LuaScriptHandle> >& uBuckets);
		static void RemoveFromBucket(const U32 bucket, const LuaScriptHandle& lh, std::vector< std::list<LuaScriptHandle> >& uBuckets);
		static std::vector< std::list<LuaScriptHandle> > sStartupBuckets;
		static std::vector< std::list<LuaScriptHandle> > sUpdateBuckets;


		static const U32 DEFAULTMAXOBJS;
		static const U32 DEFAULTMAXBUCKS;

		static bool initialized;

		static LuaScript* sFirstFree;
		static LuaScript* sLUAScriptPool;

		static ErrorManager mErrorManager;
	};

	template <class T>
	void LuaScript::addudata(const char * name, const char * udataName, T dataToCopy)
	{
		T* n = static_cast<T*>(lua_newuserdata(mL, sizeof(T)));
		luaL_getmetatable(mL, udataName);
		lua_setmetatable(mL, -2);
		*n = dataToCopy;
		lua_setglobal(mL, name);
	}
}