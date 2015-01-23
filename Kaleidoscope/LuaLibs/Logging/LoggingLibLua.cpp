#include <LuaLibs/Logging/LoggingLibLua.h>

#include <LuaLibs/Utility/stackDumpLua.h>

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static int lua_format(lua_State* L)
{
	U32 i = lua_gettop(L);
	lua_getglobal(L, "string");
	lua_getfield(L, -1, "format");
	lua_remove(L, -2);
	lua_insert(L, 1);
	if (lua_pcall(L, i, 1, 0) != LUA_OK)
	{
		gLogManager.log("error running log: %s", lua_tostring(L, -1));
	}
	return 1;
}

static int lua_log(lua_State* L)
{
	lua_format(L);
	if (lua_type(L, -1) == LUA_TSTRING)
	{
		gLogManager.log("%s", lua_tostring(L, -1));
	}

	return 0;
}

static const struct luaL_Reg logging[] = 
{
	{ "log", lua_log },
	{NULL, NULL}
};

int kaleidoscope::luaopen_klogging(lua_State* L)
{
	luaL_newlib(L, logging);
	lua_setglobal(L, "kLogging");
	return 0;
}