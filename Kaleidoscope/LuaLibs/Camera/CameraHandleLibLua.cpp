#include <LuaLibs/Camera/CameraHandleLibLua.h>

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#include <LuaLibs/Utility/lua_getters.h>
#include <LuaLibs/Utility/lua_old_typenames.h>

#include <Components/Camera/Camera.h>
#include <Components/Camera/CameraHandle.h>

using kaleidoscope::Camera;
using kaleidoscope::CameraHandle;

static int lua_ch_equal(lua_State* L)
{
	CameraHandle* lhs = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);
	CameraHandle* rhs = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);

	lua_pushboolean(L, (*lhs == *rhs));

	return 1;
}

static int lua_ch_valid(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);

	lua_pushboolean(L, ch->valid());

	return 1;
}

static int lua_ch_getName(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);

	lua_pushnumber(L, ch->getName());

	return 1;
}

static int lua_ch_getNearClipPlane(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);

	lua_pushnumber(L, ch->getNearClipPlane());

	return 1;
}

static int lua_ch_setNearClipPlane(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);
	F32 ncp = static_cast<F32>(luaL_checknumber(L, 2));

	ch->setNearClipPlane(ncp);

	return 0;
}

static int lua_ch_getFarClipPlane(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);

	lua_pushnumber(L, ch->getFarClipPlane());

	return 1;
}

static int lua_ch_setFarClipPlane(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);
	F32 fcp = static_cast<F32>(luaL_checknumber(L, 2));

	ch->setFarClipPlane(fcp);

	return 0;
}

static int lua_ch_getFieldOfView(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);

	lua_pushnumber(L, ch->getFieldOfView());

	return 1;
}

static int lua_ch_setFieldOfView(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);
	F32 fov = static_cast<F32>(luaL_checknumber(L, 2));

	ch->setFieldOfView(fov);

	return 0;
}

static int lua_ch_printState(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);

	ch->printState();

	return 0;
}

static int lua_c_addnullHandle(lua_State* L)
{
	CameraHandle* null = newudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME);
	*null = CameraHandle::null;
	lua_setfield(L, 2, "null");
	return 0;
}

static int lua_c_addNAME(lua_State* L)
{
	lua_pushnumber(L, CameraHandle::NAME);
	lua_setfield(L, 2, "NAME");
	return 0;
}

static const struct luaL_Reg cameraHandle_sf[] =
{
	{ NULL, NULL }
};

static const struct luaL_Reg cameraHandle_mf[] =
{
	{ "__eq", lua_ch_equal },
	{ "valid", lua_ch_valid },
	{ "getName", lua_ch_getName },
	{ "getNearClipPlane", lua_ch_getNearClipPlane },
	{ "setNearClipPlane", lua_ch_setNearClipPlane },
	{ "getFarClipPlane", lua_ch_getFarClipPlane },
	{ "setFarClipPlane", lua_ch_setFarClipPlane },
	{ "getFieldOfView", lua_ch_getFieldOfView },
	{ "setFieldOfView", lua_ch_setFieldOfView },
	{ "printState", lua_ch_printState },
	{ NULL, NULL }
};

int kaleidoscope::luaopen_CameraHandle(lua_State* L)
{
	luaL_newmetatable(L, kaleidoscope::CameraHandle::LUA_TYPE_NAME);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, cameraHandle_mf, 0);

	luaL_newlib(L, cameraHandle_sf);
	lua_c_addnullHandle(L);
	lua_c_addNAME(L);
	lua_setglobal(L, "CameraHandle");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}