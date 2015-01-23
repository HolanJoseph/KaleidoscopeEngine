#include <LuaLibs/Application/ApplicationLibLua.h>

extern bool gRunning;

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static int lua_quit(lua_State* L)
{
	gRunning = false;
	return 0;
}

static const struct luaL_Reg application[] =
{
	{ "quit", lua_quit },
	{ NULL, NULL }
};

int kaleidoscope::luaopen_kApplication(lua_State* L)
{
	luaL_newlib(L, application);
	lua_setglobal(L, "kApplication");
	return 0;
}