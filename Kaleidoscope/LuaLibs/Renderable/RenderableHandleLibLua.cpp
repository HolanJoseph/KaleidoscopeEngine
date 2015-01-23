#include <LuaLibs/Renderable/RenderableHandleLibLua.h>

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

#include <Components/Renderable/Renderable.h>
#include <Components/Renderable/RenderableHandle.h>

using kaleidoscope::RenderableHandle;
using kaleidoscope::Renderable;
using kaleidoscope::StringID;

static int lua_rhequal(lua_State* L)
{
	RenderableHandle* lhs = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	RenderableHandle* rhs = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 2);
	lua_pushboolean(L, (*lhs == *rhs));
	return 1;
}

static int lua_rhvalid(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	lua_pushboolean(L, rh->valid());
	return 1;
}

static int lua_rhgetName(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	lua_pushnumber(L, rh->getName());
	return 1;
}

static int lua_rhsetMesh(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	U32 mesh = static_cast<U32>(luaL_checknumber(L, 2));

	rh->setMesh(mesh);

	return 0;
}

static int lua_rhremoveMesh(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	rh->removeMesh();

	return 0;
}

static int lua_rh_visible(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	lua_pushboolean(L, rh->isEnabled());
	return 1;
}

static int lua_rh_makeVisible(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	rh->enable();
	return 0;
}

static int lua_rh_makeInvisible(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	rh->disable();
	return 0;
}

static int lua_rh_numMaterials(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	lua_pushnumber(L, rh->numMaterials());
	return 1;
}

static int lua_rh_setMaterialShader(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	U32 matNum = luaL_checkunsigned(L, 2);
	StringID shader = luaL_checkunsigned(L, 3);

	rh->setMaterialShader(matNum, shader);

	return 0;
}

static int lua_rh_getMaterialShader(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	U32 matNum = luaL_checkunsigned(L, 2);

	lua_pushunsigned(L, rh->getMaterialShader(matNum));

	return 1;
}

static int lua_rh_setMaterialAlbedo(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	U32 matNum = luaL_checkunsigned(L, 2);
	StringID path = luaL_checkunsigned(L, 3);

	rh->setMaterialAlbedo(matNum, path);

	return 0;
}

static int lua_rh_setMaterialShininess(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	U32 matNum = luaL_checkunsigned(L, 2);
	F32 val = static_cast<F32>(luaL_checknumber(L, 3));

	rh->setMaterialShininess(matNum, val);

	return 0;
}

static int lua_rh_getMaterialShininess(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	U32 matNum = luaL_checkunsigned(L, 2);

	lua_pushnumber(L, rh->getMaterialShininess(matNum));

	return 1;
}

static int lua_rh_setMaterialSpecularColor(lua_State* L)
{
	using kaleidoscope::math::vec4;

	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	U32 matNum = luaL_checkunsigned(L, 2);
	vec4* c = getudata<vec4>(L, vec4TypeName, 3);

	rh->setMaterialSpecularColor(matNum, *c);

	return 0;
}

static int lua_rh_getMaterialSpecularColor(lua_State* L)
{
	using kaleidoscope::math::vec4;

	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	U32 matNum = luaL_checkunsigned(L, 2);

	vec4* c = newudata<vec4>(L, vec4TypeName);
	*c = rh->getMaterialSpecularColor(matNum);

	return 1;
}

static int lua_rhsetAlbedo(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	U32 albedoTex = static_cast<U32>(luaL_checknumber(L, 2));

	rh->setAlbedo(albedoTex);

	return 0;
}

static int lua_rhprintState(lua_State* L)
{
	RenderableHandle* rh = getudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME, 1);
	rh->printState();
	return 0;
}

static int lua_raddNullHandle(lua_State* L)
{
	RenderableHandle* null = newudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME);
	*null = RenderableHandle::null;
	lua_setfield(L, 2, "null");
	return 0;
}

static int lua_raddNAME(lua_State* L)
{
	lua_pushnumber(L, RenderableHandle::NAME);
	lua_setfield(L, 2, "NAME");
	return 0;
}

static const struct luaL_Reg renderableHandle_sf[] =
{
	{ NULL, NULL }
};

static const struct luaL_Reg renderableHandle_mf[] =
{
	{ "__eq", lua_rhequal },
	{ "valid", lua_rhvalid },
	{ "getName", lua_rhgetName },
	{ "setMesh", lua_rhsetMesh },
	{ "removeMesh", lua_rhremoveMesh },
	{ "isEnabled", lua_rh_visible },
	{ "enable", lua_rh_makeVisible },
	{ "disable", lua_rh_makeInvisible },
	{ "numMaterials", lua_rh_numMaterials },
	{ "setMaterialShader", lua_rh_setMaterialShader },
	{ "getMaterialShader", lua_rh_getMaterialShader },
	{ "setMaterialAlbedo", lua_rh_setMaterialAlbedo },
	{ "setMaterialShininess", lua_rh_setMaterialShininess },
	{ "getMaterialShininess", lua_rh_getMaterialShininess },
	{ "setMaterialSpecularColor", lua_rh_setMaterialSpecularColor },
	{ "getMaterialSpecularColor", lua_rh_getMaterialSpecularColor },
	{ "setAlbedo", lua_rhsetAlbedo },
	{ "printState", lua_rhprintState },
	{ NULL, NULL }
};

int kaleidoscope::luaopen_RenderableHandle(lua_State* L)
{
	luaL_newmetatable(L, kaleidoscope::RenderableHandle::LUA_TYPE_NAME);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, renderableHandle_mf, 0);

	luaL_newlib(L, renderableHandle_sf);
	lua_raddNullHandle(L);
	lua_raddNAME(L);
	lua_setglobal(L, "RenderableHandle");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}