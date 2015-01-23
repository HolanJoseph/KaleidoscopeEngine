#include <LuaLibs/Light/LightHandleLibLua.h>

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

#include <Components/Light/Light.h>
#include <Components/Light/LightHandle.h>

using kaleidoscope::Light;
using kaleidoscope::LightHandle;

static int lua_lh_equal(lua_State* L)
{
	LightHandle* lhs = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	LightHandle* rhs = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 2);
	lua_pushboolean(L, (*lhs == *rhs));
	return 1;
}

static int lua_lh_valid(lua_State* L)
{
	LightHandle* rh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	lua_pushboolean(L, rh->valid());
	return 1;
}

static int lua_lh_getName(lua_State* L)
{
	LightHandle* rh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	lua_pushnumber(L, rh->getName());
	return 1;
}



static int lua_lh_setVisible(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	lh->enable();
	return 0;
}

static int lua_lh_setInvisible(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	lh->disable();
	return 0;
}

static int lua_lh_isVisible(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	lua_pushboolean(L, lh->isEnabled());
	return 1;
}

static int lua_lh_setLightType(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	kaleidoscope::LightHandle::LightType lt;
	I32 llt = static_cast<I32>(luaL_checknumber(L, 2));
	lt = static_cast<kaleidoscope::LightHandle::LightType>(llt);

	lh->setLightType(lt);

	return 0;
}

static int lua_lh_getLightType(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);

	lua_pushnumber(L, lh->getLightType());

	return 1;
}

static int lua_lh_setRadius(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	F32 r = static_cast<F32>(luaL_checknumber(L, 2));
	lh->setRadius(r);
	return 0;
}

static int lua_lh_getRadius(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	lua_pushnumber(L, lh->getRadius());
	return 1;
}

using kaleidoscope::math::vec4;
using kaleidoscope::math::vec3;

static int lua_lh_setAmbientColor(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	vec4* c = getudata<vec4>(L, vec4TypeName, 2);

	lh->setAmbientColor(*c);

	return 0;
}

static int lua_lh_getAmbientColor(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);

	vec4* c = newudata<vec4>(L, vec4TypeName);
	*c = lh->getAmbientColor();

	return 1;
}

static int lua_lh_setDiffuseColor(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	vec4* c = getudata<vec4>(L, vec4TypeName, 2);

	lh->setDiffuseColor(*c);

	return 0;
}

static int lua_lh_getDiffuseColor(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);

	vec4* c = newudata<vec4>(L, vec4TypeName);
	*c = lh->getDiffuseColor();

	return 1;
}

static int lua_lh_setSpecularColor(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	vec4* c = getudata<vec4>(L, vec4TypeName, 2);

	lh->setSpecularColor(*c);
	return 0;
}

static int lua_lh_getSpecularColor(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);

	vec4* c = newudata<vec4>(L, vec4TypeName);
	*c = lh->getSpecularColor();

	return 1;
}

static int lua_lh_setAttenuation(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	vec3* atten = getudata<vec3>(L, vec3TypeName, 2);

	lh->setAttenuation(*atten);

	return 0;
}

static int lua_lh_getAttenuation(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);

	vec3* atten = newudata<vec3>(L, vec3TypeName);
	*atten = lh->getAttenuation();

	return 1;
}

static int lua_lh_setInnerCone(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	F32 ic = static_cast<F32>(luaL_checknumber(L, 2));
	lh->setInnerCone(ic);
	return 0;
}

static int lua_lh_getInnerCone(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	lua_pushnumber(L, lh->getInnerCone());
	return 1;
}

static int lua_lh_setOuterCone(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	F32 oc = static_cast<F32>(luaL_checknumber(L, 2));
	lh->setOuterCone(oc);
	return 0;
}

static int lua_lh_getOuterCone(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	lua_pushnumber(L, lh->getOuterCone());
	return 1;
}

static int lua_lh_setFalloff(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	F32 f = static_cast<F32>(luaL_checknumber(L, 2));
	lh->setFalloff(f);
	return 0;
}

static int lua_lh_getFalloff(lua_State* L)
{
	LightHandle* lh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	lua_pushnumber(L, lh->getFalloff());
	return 1;
}



static int lua_lh_printState(lua_State* L)
{
	LightHandle* rh = getudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME, 1);
	rh->printState();
	return 0;
}

static int lua_l_addNullHandle(lua_State* L)
{
	LightHandle* null = newudata<LightHandle>(L, LightHandle::LUA_TYPE_NAME);
	*null = LightHandle::null;
	lua_setfield(L, 2, "null");
	return 0;
}

static int lua_l_addNAME(lua_State* L)
{
	lua_pushnumber(L, LightHandle::NAME);
	lua_setfield(L, 2, "NAME");
	return 0;
}

static int lua_l_addLightType(lua_State* L)
{
	lua_newtable(L);
	addEnum(L, "POINT", kaleidoscope::LightHandle::POINT_LIGHT);
	addEnum(L, "SPOT", kaleidoscope::LightHandle::SPOT_LIGHT);
	addEnum(L, "DIRECTIONAL", kaleidoscope::LightHandle::DIRECTIONAL_LIGHT);
	lua_setfield(L, 1, "LightType");
	return 0;

}

static const struct luaL_Reg lightHandle_sf[] =
{
	{ NULL, NULL }
};

static const struct luaL_Reg lightHandle_mf[] =
{
	{ "__eq", lua_lh_equal },
	{ "valid", lua_lh_valid },
	{ "getName", lua_lh_getName },
	{ "enable", lua_lh_setVisible },
	{ "disable", lua_lh_setInvisible },
	{ "isEnabled", lua_lh_isVisible },
	{ "setLightType", lua_lh_setLightType },
	{ "getLightType", lua_lh_getLightType },
	{ "setRadius", lua_lh_setRadius },
	{ "getRadius", lua_lh_getRadius },
	{ "setAmbientColor", lua_lh_setAmbientColor },
	{ "getAmbientColor", lua_lh_getAmbientColor },
	{ "setDiffuseColor", lua_lh_setDiffuseColor },
	{ "getDiffuseColor", lua_lh_getDiffuseColor },
	{ "setSPecularColor", lua_lh_setSpecularColor },
	{ "getSpecularColor", lua_lh_getSpecularColor },
	{ "setAttenuation", lua_lh_setAttenuation },
	{ "getAttenuation", lua_lh_getAttenuation },
	{ "setInnerCone", lua_lh_setInnerCone },
	{ "getInnerCone", lua_lh_getInnerCone },
	{ "setOuterCone", lua_lh_setOuterCone },
	{ "getOuterCone", lua_lh_getOuterCone },
	{ "setFalloff", lua_lh_setFalloff },
	{ "getFalloff", lua_lh_getFalloff },
	{ "printState", lua_lh_printState },
	{ NULL, NULL }
};

int kaleidoscope::luaopen_LightHandle(lua_State* L)
{
	luaL_newmetatable(L, kaleidoscope::LightHandle::LUA_TYPE_NAME);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, lightHandle_mf, 0);

	luaL_newlib(L, lightHandle_sf);
	lua_l_addNullHandle(L);
	lua_l_addNAME(L);
	lua_l_addLightType(L);
	lua_setglobal(L, "LightHandle");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}