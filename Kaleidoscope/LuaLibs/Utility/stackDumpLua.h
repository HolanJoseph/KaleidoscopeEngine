#pragma once

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}


#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;

inline void stackDump(lua_State* L)
{
	int i;
	int top = lua_gettop(L); /* depth of the stack*/
	for (i = 1; i <= top; ++i) /* repeat for each level */
	{
		int t = lua_type(L, i);
		switch (t)
		{
		case LUA_TSTRING: 
			gLogManager.log("'%s'", lua_tostring(L, i));
			break;
		case LUA_TBOOLEAN:
			gLogManager.log("%s", (lua_toboolean(L, i) ? "true" : "false"));
			break;
		case LUA_TNUMBER:
			gLogManager.log("%g", lua_tonumber(L, i));
			break;
		default:
			gLogManager.log("%s", lua_typename(L, t));
			break;
		}
		gLogManager.log("");
	}
}