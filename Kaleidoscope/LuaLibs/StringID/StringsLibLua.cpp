#include <LuaLibs/StringID/StringsLibLua.h>

#include <Utility/StringID/StringId.h>

#include <LuaLibs/Utility/stackDumpLua.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static int lua_getString(lua_State* L)
{
	kaleidoscope::StringID s = luaL_checkunsigned(L, 1);
	lua_pushstring(L, kaleidoscope::getString(s));
	return 1;
}

static int lua_internString(lua_State* L)
{
	if (lua_type(L, 1) != LUA_TSTRING)
	{
		gLogManager.log("getString: argument is not a string");
		return 0;
	}
	const char * s = lua_tostring(L, 1);
	lua_pushnumber(L, kaleidoscope::internString(s));
	return 1;
}

static const struct luaL_Reg kstringID[] =
{
	{ "getString", lua_getString },
	{ "internString", lua_internString },
	{ NULL, NULL }
};


int kaleidoscope::luaopen_stringID(lua_State* L)
{
	luaL_newlib(L, kstringID);
	lua_setglobal(L, "kStringID");
	return 0;
}