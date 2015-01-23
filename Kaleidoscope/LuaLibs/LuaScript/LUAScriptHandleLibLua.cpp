#include <LuaLibs/LuaScript/LUAScriptHandleLibLua.h>

#include <Components/LuaScript/LuaScriptHandle.h>
#include <GameObject/GameObjectHandle.h>
#include <Event/Event.h>
#include <Components/LuaScript/LuaScript.h>

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

using kaleidoscope::LuaScriptHandle;
using kaleidoscope::StringID;
using kaleidoscope::Event;
using kaleidoscope::GameObjectHandle;
using kaleidoscope::math::vec3;
using kaleidoscope::math::vec4;
using kaleidoscope::math::mat4;
using kaleidoscope::math::quat;

static const char * luaScriptHandleTypeName = "kaleidoscope.LUAScriptHandle";
static const char * gameObjectHandleTypeName = "kaleidoscope.GameObjectHandle";
static const char * eventTypeName = "kaleidoscope.event";
static const char * vec3TypeName = "kaleidoscope.vec3";
static const char * vec4TypeName = "kaleidoscope.vec4";
static const char * mat4TypeName = "kaleidoscope.mat4";
static const char * quatTypeName = "kaleidoscope.quat";


static LuaScriptHandle* newLUAScriptHandle(lua_State* L)
{
	LuaScriptHandle* n = static_cast<LuaScriptHandle*>(lua_newuserdata(L, sizeof(LuaScriptHandle)));
	luaL_getmetatable(L, luaScriptHandleTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static LuaScriptHandle* getLUAScriptHandle(lua_State* L, U32 index)
{
	return static_cast<LuaScriptHandle*>(luaL_checkudata(L, index, luaScriptHandleTypeName));
}

static GameObjectHandle* newGameObjectHandle(lua_State* L)
{
	GameObjectHandle* n = static_cast<GameObjectHandle*>(lua_newuserdata(L, sizeof(GameObjectHandle)));
	luaL_getmetatable(L, gameObjectHandleTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static GameObjectHandle* getGameObjectHandle(lua_State* L, U32 index)
{
	return static_cast<GameObjectHandle*>(luaL_checkudata(L, index, gameObjectHandleTypeName));
}

static Event* newEvent(lua_State* L)
{
	Event* n = static_cast<Event*>(lua_newuserdata(L, sizeof(Event)));
	luaL_getmetatable(L, eventTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static Event* getEvent(lua_State* L, U32 index)
{
	return static_cast<Event*>(luaL_checkudata(L, index, eventTypeName));
}

static vec3* newvec3(lua_State* L)
{
	vec3* n = static_cast<vec3*>(lua_newuserdata(L, sizeof(vec3)));
	luaL_getmetatable(L, vec3TypeName);
	lua_setmetatable(L, -2);
	return n;
}

static vec3* getvec3(lua_State* L, U32 index)
{
	return static_cast<vec3*>(luaL_checkudata(L, index, vec3TypeName));
}

static vec4* newvec4(lua_State* L)
{
	vec4* n = static_cast<vec4*>(lua_newuserdata(L, sizeof(vec4)));
	luaL_getmetatable(L, vec4TypeName);
	lua_setmetatable(L, -2);
	return n;
}

static vec4* getvec4(lua_State* L, U32 index)
{
	return static_cast<vec4*>(luaL_checkudata(L, index, vec4TypeName));
}

static mat4* newmat4(lua_State* L)
{
	mat4* n = static_cast<mat4*>(lua_newuserdata(L, sizeof(mat4)));
	luaL_getmetatable(L, mat4TypeName);
	lua_setmetatable(L, -2);
	return n;
}

static mat4* getmat4(lua_State* L, U32 index)
{
	return static_cast<mat4*>(luaL_checkudata(L, index, mat4TypeName));
}

static quat* newquat(lua_State* L)
{
	quat* n = static_cast<quat*>(lua_newuserdata(L, sizeof(quat)));
	luaL_getmetatable(L, quatTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static quat* getquat(lua_State* L, U32 index)
{
	return static_cast<quat*>(luaL_checkudata(L, index, quatTypeName));
}


static StringID getstringid(lua_State* L, U32 index)
{
	return static_cast<StringID>(luaL_checknumber(L, index));
}

static int lua_lhvalid(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	lua_pushboolean(L, lh->valid());
	return 1;
}

static int lua_lhgetname(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	lua_pushnumber(L, lh->getName());
	return 1;
}

static int lua_lhenabled(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	lua_pushboolean(L, lh->isEnabled());
	return 1;
}

static int lua_lhenable(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	lh->enable();
	return 0;
}

static int lua_lhdisable(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	lh->disable();
	return 0;
}

static int lua_lhgetfilename(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	lua_pushnumber(L, lh->getFileName());
	return 1;
}

static int lua_lhgetglobalnumber(lua_State* L)
{
	bool b = false;
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	lua_pushnumber(L, lh->getGlobalNumber(name, b));
	lua_pushboolean(L, b);
	return 2;
}

static int lua_lhgetglobalbool(lua_State* L)
{
	bool b = false;
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	lua_pushnumber(L, lh->getGlobalBool(name, b));
	lua_pushboolean(L, b);
	return 2;
}

static int lua_lhgetglobalstringid(lua_State* L)
{
	bool b = false;
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	lua_pushnumber(L, lh->getGlobalStringID(name, b));
	lua_pushboolean(L, b);
	return 2;
}

static int lua_lhgetglobalvec3(lua_State* L)
{
	bool b = false;
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	vec3* n = newvec3(L);
	*n = lh->getGlobalvec3(name, b);
	lua_pushboolean(L, b);
	return 2;
}

static int lua_lhgetglobalvec4(lua_State* L)
{
	bool b = false;
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	vec4* n = newvec4(L);
	*n = lh->getGlobalvec4(name, b);
	lua_pushboolean(L, b);
	return 2;
}

static int lua_lhgetglobalmat4(lua_State* L)
{
	bool b = false;
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	mat4* n = newmat4(L);
	*n = lh->getGlobalmat4(name, b);
	lua_pushboolean(L, b);
	return 2;
}

static int lua_lhgetglobalquat(lua_State* L)
{
	bool b = false;
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	quat* n = newquat(L);
	*n = lh->getGlobalquat(name, b);
	lua_pushboolean(L, b);
	return 2;
}

static int lua_lhgetglobalgameobjecthandle(lua_State* L)
{
	bool b = false;
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	GameObjectHandle* n = newGameObjectHandle(L);
	*n = lh->getGlobalGameObjectHandle(name, b);
	lua_pushboolean(L, b);
	return 2;
}

static int lua_lhsetglobalnumber(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	F32 value = static_cast<F32>(luaL_checknumber(L, 3));
	lh->setGlobalNumber(name, value);
	return 0;
}

static int lua_lhsetglobalbool(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	//bool value = static_cast<bool>(lua_toboolean(L, 3));
	bool value = (lua_toboolean(L, 3) != 0);
	lh->setGlobalBool(name, value);
	return 0;
}

static int lua_lhsetglobalstringid(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	StringID value = getstringid(L, 3);
	lh->setGlobalStringID(name, value);
	return 0;
}

static int lua_lhsetglobalvec3(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	vec3* value = getvec3(L, 3);
	lh->setGlobalvec3(name, *value);
	return 0;
}

static int lua_lhsetglobalvec4(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	vec4* value = getvec4(L, 3);
	lh->setGlobalvec4(name, *value);
	return 0;
}

static int lua_lhsetglobalmat4(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	mat4* value = getmat4(L, 3);
	lh->setGlobalmat4(name, *value);
	return 0;
}

static int lua_lhsetglobalquat(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	quat* value = getquat(L, 3);
	lh->setGlobalquat(name, *value);
	return 0;
}

static int lua_lhsetglobalgameobjecthandle(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	const char * name = luaL_checkstring(L, 2);
	GameObjectHandle* value = getGameObjectHandle(L, 3);
	lh->setGlobalGameObjectHandle(name, *value);
	return 0;
}

static int lua_lhprintstate(lua_State* L)
{
	LuaScriptHandle* lh = getLUAScriptHandle(L, 1);
	lh->printState();
	return 0;
}

static int lua_lhequal(lua_State* L)
{
	LuaScriptHandle* lhs = getLUAScriptHandle(L, 1);
	LuaScriptHandle* rhs = getLUAScriptHandle(L, 2);
	lua_pushboolean(L, (*lhs == *rhs));
	return 1;
}

static int lua_lhaddnullhandle(lua_State* L)
{
	LuaScriptHandle* null = newLUAScriptHandle(L);
	*null = kaleidoscope::LuaScriptHandle::null;
	lua_setfield(L, 2, "null");
	return 0;
}

static int lua_lhaddname(lua_State* L)
{
	lua_pushnumber(L, kaleidoscope::LuaScriptHandle::NAME);
	lua_setfield(L, 2, "NAME");
	return 0;
}


static const struct luaL_Reg luascripthandle_sf[] = 
{
	{ NULL, NULL }
};

static const struct luaL_Reg luascripthandle_mf[] = 
{
	{ "valid", lua_lhvalid },
	{ "getName", lua_lhgetname },
	{ "isEnabled", lua_lhenabled },
	{ "enable", lua_lhenable },
	{ "disable", lua_lhdisable },
	{ "getFileName", lua_lhgetfilename },
	{ "getGlobalNumber", lua_lhgetglobalnumber },
	{ "getGlobalBool", lua_lhgetglobalbool },
	{ "getGlobalStringID", lua_lhgetglobalstringid },
	{ "getGlobalVec3", lua_lhgetglobalvec3 },
	{ "getGlobalVec4", lua_lhgetglobalvec4 },
	{ "getGlobalMat4", lua_lhgetglobalmat4 },
	{ "getGlobalQuat", lua_lhgetglobalquat },
	{ "getGlobalGameObjectHandle", lua_lhgetglobalgameobjecthandle },
	{ "setGlobalNumber", lua_lhsetglobalnumber },
	{ "setGlobalBool", lua_lhsetglobalbool },
	{ "setGlobalStringID", lua_lhsetglobalstringid },
	{ "setGlobalVec3", lua_lhsetglobalvec3 },
	{ "setGlobalVec4", lua_lhsetglobalvec4 },
	{ "setGlobalQuat", lua_lhsetglobalquat },
	{ "setGlobalGameObjectHandle", lua_lhsetglobalgameobjecthandle },
	{ "printState", lua_lhprintstate },
	{ "__eq", lua_lhequal },
	{ NULL, NULL }
};

int kaleidoscope::luaopen_LUAScriptHandle(lua_State* L)
{
	luaL_newmetatable(L, luaScriptHandleTypeName);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, luascripthandle_mf, 0);

	luaL_newlib(L, luascripthandle_sf);
	lua_lhaddnullhandle(L);
	lua_lhaddname(L);
	lua_setglobal(L, "LUAScriptHandle");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}