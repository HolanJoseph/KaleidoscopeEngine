#include <LuaLibs/Transform/TransformHandleLibLua.h>

#include <Components/Transform/Transform.h>
#include <Components/Transform/TransformHandle.h>

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static const char * transformHandleTypeName = "kaleidoscope.transformHandle";
static const char * vec3TypeName = "kaleidoscope.vec3";
static const char * mat4TypeName = "kaleidoscope.mat4";
static const char * quatTypeName = "kaleidoscope.quat";

using kaleidoscope::TransformHandle;
using kaleidoscope::math::vec3;
using kaleidoscope::math::mat4;
using kaleidoscope::math::quat;
using kaleidoscope::StringID;

static TransformHandle* newtransformhandle(lua_State* L)
{
	TransformHandle* n = static_cast<TransformHandle*>(lua_newuserdata(L, sizeof(TransformHandle)));
	luaL_getmetatable(L, transformHandleTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static TransformHandle* gettransformhandle(lua_State* L, U32 index)
{
	return static_cast<TransformHandle*>(luaL_checkudata(L, index, transformHandleTypeName));
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

static kaleidoscope::StringID getstringid(lua_State* L, U32 index)
{
	return static_cast<kaleidoscope::StringID>(luaL_checkunsigned(L, index));
}

static F32 getF32(lua_State* L, U32 index)
{
	return static_cast<F32>(luaL_checknumber(L, index));
}

static int lua_thvalid(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	lua_pushboolean(L, th->valid());
	return 1;
}

static int lua_thgetparent(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	TransformHandle* n = newtransformhandle(L);
	*n = th->getParent();
	return 1;
}

static int lua_thsetparent(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	TransformHandle* p = gettransformhandle(L, 2);
	th->setParent(*p);
	return 0;
}

static int lua_thgetchild(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	StringID name = getstringid(L, 2);
	TransformHandle* n = newtransformhandle(L);
	*n = th->getChild(name);
	return 1;
}

static int lua_thaddchild(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	TransformHandle* child = gettransformhandle(L, 2);
	th->addChild(*child);
	return 0;
}

static int lua_thremovechild(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	TransformHandle* child = gettransformhandle(L, 2);
	th->removeChild(*child);
	return 0;
}

static int lua_thgetlocalposition(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* n = newvec3(L);
	*n = th->getLocalPosition();
	return 1;
}

static int lua_thsetlocalposition(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* npos = getvec3(L, 2);
	th->setLocalPosition(*npos);
	return 0;
}

static int lua_thtranslate(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* trans = getvec3(L, 2);
	th->translate(*trans);
	return 0;
}

static int lua_thgetlocalscale(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* n = newvec3(L);
	*n = th->getLocalScale();
	return 1;
}

static int lua_thgetlocalxscale(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	lua_pushnumber(L, th->getLocalXScale());
	return 1;
}

static int lua_thgetlocalyscale(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	lua_pushnumber(L, th->getLocalYScale());
	return 1;
}

static int lua_thgetlocalzscale(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	lua_pushnumber(L, th->getLocalZScale());
	return 1;
}

static int lua_thsetlocalscale(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* nscale = getvec3(L, 2);
	th->setLocalScale(*nscale);
	return 0;
}

static int lua_thsetlocalxscale(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	F32 xscale = getF32(L, 2);
	th->setLocalXScale(xscale);
	return 0;
}

static int lua_thsetlocalyscale(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	F32 yscale = getF32(L, 2);
	th->setLocalYScale(yscale);
	return 0;
}

static int lua_thsetlocalzscale(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	F32 zscale = getF32(L, 2);
	th->setLocalZScale(zscale);
	return 0;
}

static int lua_thgetlocalorientation(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	quat* ori = newquat(L);
	*ori = th->getLocalOrientation();
	return 1;
}

static int lua_thsetlocalorientation(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	quat* nori = getquat(L, 2);
	th->setLocalOrientation(*nori);
	return 0;
}

static int lua_throtatearoundaxislocal(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* axis = getvec3(L, 2);
	F32 angle = getF32(L, 3);
	th->rotateAroundAxisLocal(*axis, angle);
	return 0;
}

static int lua_throtatearoundaxisworld(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* axis = getvec3(L, 2);
	F32 angle = getF32(L, 3);
	th->rotateAroundAxisWorld(*axis, angle);
	return 0;
}

static int lua_thgetworldorientation(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	quat* ori = newquat(L);
	*ori = th->getWorldOrientation();
	return 1;
}

static int lua_thgetworldscale(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* scale = newvec3(L);
	*scale = th->getWorldScale();
	return 1;
}

static int lua_thgetworldposition(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* pos = newvec3(L);
	*pos = th->getWorldPosition();
	return 1;
}

static int lua_thsetworldposition(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* npos = getvec3(L, 2);
	th->setWorldPosition(*npos);
	return 0;
}

static int lua_throtatearoundworldpoint(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	vec3* pt = getvec3(L, 2);
	vec3* axis = getvec3(L, 3);
	F32 theta = getF32(L, 4);
	th->rotateAroundWorldPoint(*pt, *axis, theta);
	return 0;
}

static int lua_thlookat(lua_State* L)
{
	if (lua_gettop(L) == 2)
	{
		TransformHandle* th = gettransformhandle(L, 1);
		vec3* pt = getvec3(L, 2);
		th->lookAt(*pt);
		return 0;
	}
	else if (lua_gettop(L) == 3)
	{
		TransformHandle* th = gettransformhandle(L, 1);
		vec3* pt = getvec3(L, 2);
		vec3* upHint = getvec3(L, 3);
		th->lookAt(*pt, *upHint);
		return 0;
	}

	return luaL_error(L, "general argument error");
}

static int lua_thgetlocaltoworldmatrix(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	mat4* ltwm = newmat4(L);
	*ltwm = th->getLocalToWorldMatrix();
	return 1;
}

static int lua_thgetworldtolocalmatrix(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	mat4* wtlm = newmat4(L);
	*wtlm = th->getWorldToLocalMatrix();
	return 1;
}

static int lua_thprintstate(lua_State* L)
{
	TransformHandle* th = gettransformhandle(L, 1);
	th->printState();
	return 0;
}

static int lua_thequal(lua_State* L)
{
	TransformHandle* lhs = gettransformhandle(L, 1);
	TransformHandle* rhs = gettransformhandle(L, 2);
	lua_pushboolean(L, (*lhs == *rhs));
	return 1;
}

static int lua_thaddnullhandle(lua_State* L)
{
	TransformHandle* null = newtransformhandle(L);
	*null = kaleidoscope::TransformHandle::null;
	lua_setfield(L, 2, "null");
	return 0;
}

static int lua_thaddname(lua_State* L)
{
	lua_pushnumber(L, kaleidoscope::TransformHandle::NAME);
	lua_setfield(L, 2, "NAME");
	return 0;
}

static const struct luaL_Reg transformhandle_sf[] = 
{
	{ NULL, NULL }
};

static const struct luaL_Reg transformhandle_mf[] = 
{
	{ "valid", lua_thvalid },
	{ "getParent", lua_thgetparent },
	{ "setParent", lua_thsetparent },
	{ "getChild", lua_thgetchild },
	{ "addChild", lua_thaddchild },
	{ "removeChild", lua_thremovechild },
	{ "getLocalPosition", lua_thgetlocalposition },
	{ "setLocalPosition", lua_thsetlocalposition },
	{ "translate", lua_thtranslate },
	{ "getLocalScale", lua_thgetlocalscale },
	{ "getLocalXScale", lua_thgetlocalxscale },
	{ "getLocalYScale", lua_thgetlocalyscale },
	{ "getLocalZScale", lua_thgetlocalzscale },
	{ "setLocalScale", lua_thsetlocalscale },
	{ "setLocalXScale", lua_thsetlocalxscale },
	{ "setLocalYScale", lua_thsetlocalyscale },
	{ "setLocalZScale", lua_thsetlocalzscale },
	{ "getLocalOrientation", lua_thgetlocalorientation },
	{ "setLocalOrientation", lua_thsetlocalorientation },
	{ "rotateAroundAxisLocal", lua_throtatearoundaxislocal },
	{ "rotateAroundAxisWorld", lua_throtatearoundaxisworld },
	{ "getWorldOrientation", lua_thgetworldorientation },
	{ "getWorldScale", lua_thgetworldscale },
	{ "getWorldPosition", lua_thgetworldposition },
	{ "setWorldPosition", lua_thsetworldposition },
	{ "rotateAroundWorldPoint", lua_throtatearoundworldpoint },
	{ "lookAt", lua_thlookat },
	{ "getLocalToWorldMatrix", lua_thgetlocaltoworldmatrix },
	{ "getWorldToLocalMatrix", lua_thgetworldtolocalmatrix },
	{ "printState", lua_thprintstate },
	{ "__eq", lua_thequal },
	{ NULL, NULL }
};

int kaleidoscope::luaopen_TransformHandle(lua_State* L)
{
	luaL_newmetatable(L, transformHandleTypeName);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, transformhandle_mf, 0);

	luaL_newlib(L, transformhandle_sf);
	lua_thaddnullhandle(L);
	lua_thaddname(L);
	lua_setglobal(L, "TransformHandle");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}