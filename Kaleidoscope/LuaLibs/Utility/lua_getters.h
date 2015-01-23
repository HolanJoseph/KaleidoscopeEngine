#pragma once

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

template <class T>
inline T* newudata(lua_State* L, const char * udataName)
{
	T* n = static_cast<T*>(lua_newuserdata(L, sizeof(T)));
	luaL_getmetatable(L, udataName);
	lua_setmetatable(L, -2);
	return n;
}

template <class T>
inline T* getudata(lua_State* L, const char * udataName, U32 index)
{
	return static_cast<T*>(luaL_checkudata(L, index, udataName));
}

inline void addEnum(lua_State* L, const char * k, I32 v)
{
	lua_pushstring(L, k);
	lua_pushnumber(L, v);
	lua_settable(L, -3);
}