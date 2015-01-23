#include <LuaLibs/Renderer/RendererLibLua.h>

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

// #include "lua_getters.h"
// #include "lua_old_typenames.h"
// 
// #include "Camera.h"
// #include "CameraHandle.h"

#include <LuaLibs/Utility/lua_getters.h>
#include <LuaLibs/Utility/lua_old_typenames.h>

#include <Components/Camera/Camera.h>
#include <Components/Camera/CameraHandle.h>
using kaleidoscope::Camera;
using kaleidoscope::CameraHandle;

//#include "RenderManager.h"
#include <Rendering/Management/RenderManager.h>
extern kaleidoscope::RenderManager gRenderManager;

static int lua_kRenderer_setViewCamera(lua_State* L)
{
	CameraHandle* ch = getudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME, 1);
	gRenderManager.setViewCamera(*ch);
	return 0;
}

static int lua_kRenderer_getViewCamera(lua_State* L)
{
	CameraHandle* n = newudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME);
	*n = gRenderManager.getViewCamera();
	return 1;
}

static int lua_kRenderer_getScreenDimensions(lua_State* L)
{
	kaleidoscope::math::vec2 res = gRenderManager.getScreenDimensions();
	lua_pushnumber(L, res.x);
	lua_pushnumber(L, res.y);
	return 2;
}

static int lua_kRenderer_enableLighting(lua_State* L)
{
	gRenderManager.enableLighting();
	return 0;
}

static int lua_kRenderer_disableLighting(lua_State* L)
{
	gRenderManager.disableLighting();
	return 0;
}

static int lua_kRenderer_isLightingEnabled(lua_State* L)
{
	lua_pushboolean(L, gRenderManager.isLightingEnabled());
	return 1;
}

static const struct luaL_Reg kRenderer_sf[] =
{
	{ "setViewCamera", lua_kRenderer_setViewCamera },
	{ "getViewCamera", lua_kRenderer_getViewCamera },
	{ "getScreenDimensions", lua_kRenderer_getScreenDimensions },
	{ "enableLighting", lua_kRenderer_enableLighting },
	{ "disableLighting", lua_kRenderer_disableLighting },
	{ "isLightingEnabled", lua_kRenderer_isLightingEnabled },
	{ NULL, NULL }
};

int kaleidoscope::luaopen_kRenderer(lua_State* L)
{
	luaL_newlib(L, kRenderer_sf);
	lua_setglobal(L, "kRenderer");
	return 0;
}