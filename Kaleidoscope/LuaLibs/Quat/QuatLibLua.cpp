#include <LuaLibs/Quat/QuatLibLua.h>

#include <Utility/Typedefs.h>
#include <Math/Math.h>

#include <cstring>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static const char * quatTypeName = "kaleidoscope.quat";
static const char * vec3TypeName = "kaleidoscope.vec3";
static const char * vec4TypeName = "kaleidoscope.vec4";

using kaleidoscope::math::vec3;
static vec3* newvec3(lua_State* L)
{
	vec3* n = static_cast<vec3*>(lua_newuserdata(L, sizeof(vec3)));
	luaL_getmetatable(L, vec3TypeName);
	lua_setmetatable(L, -2);
	return n;
}

using kaleidoscope::math::vec4;
static vec4* newvec4(lua_State* L)
{
	vec4* n = static_cast<vec4*>(lua_newuserdata(L, sizeof(vec4)));
	luaL_getmetatable(L, vec4TypeName);
	lua_setmetatable(L, -2);
	return n;
}


using kaleidoscope::math::quat;
static quat* newquat(lua_State* L)
{
	quat* n = static_cast<quat*>(lua_newuserdata(L, sizeof(quat)));
	luaL_getmetatable(L, quatTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static int lua_quatcrossquatandquat(lua_State* L)
{
	kaleidoscope::math::quat* v1 = static_cast<kaleidoscope::math::quat*>(luaL_checkudata(L, 1, "kaleidoscope.quat"));
	kaleidoscope::math::quat* v2 = static_cast<kaleidoscope::math::quat*>(luaL_checkudata(L, 2, "kaleidoscope.quat"));
	quat* n = newquat(L);
	*n = kaleidoscope::math::cross(*v1, *v2);
	return 1;
}

using kaleidoscope::math::vec3;
using kaleidoscope::math::quat;
static int lua_quatcrossquatandvec3(lua_State* L)
{
	quat* v1 = static_cast<quat*>(luaL_checkudata(L, 1, "kaleidoscope.quat"));
	vec3* v2 = static_cast<vec3*>(luaL_checkudata(L, 2, "kaleidoscope.vec3"));
	vec3* n = newvec3(L);
	*n = kaleidoscope::math::cross(*v1, *v2);
	return 1;
}

static int lua_quatcross(lua_State* L)
{
	if ((luaL_testudata(L, 1, "kaleidoscope.quat")) && (luaL_testudata(L, 2, "kaleidoscope.quat")))
	{
		return lua_quatcrossquatandquat(L);
	}
	else if ((luaL_testudata(L, 1, "kaleidoscope.quat")) && (luaL_testudata(L, 2, "kaleidoscope.vec3")))
	{
		return lua_quatcrossquatandvec3(L);
	}

	return luaL_error(L, "error incorrect args");
}


static int lua_quatnormalize(lua_State* L)
{
	quat* v1 = static_cast<quat*>(luaL_checkudata(L, 1, "kaleidoscope.quat"));
	quat* n = newquat(L);
	*n = kaleidoscope::math::normalize(*v1);
	return 1;
}


static int lua_quatslerp(lua_State* L)
{
	quat* v1 = static_cast<quat*>(luaL_checkudata(L, 1, "kaleidoscope.quat"));
	quat* v2 = static_cast<quat*>(luaL_checkudata(L, 2, "kaleidoscope.quat"));
	F32 t = static_cast<F32>(luaL_checknumber(L, 3));
	quat* n = newquat(L);
	*n = kaleidoscope::math::slerp(*v1, *v2, t);
	return 1;
}


static int lua_quatlerp(lua_State* L)
{
	quat* v1 = static_cast<quat*>(luaL_checkudata(L, 1, "kaleidoscope.quat"));
	quat* v2 = static_cast<quat*>(luaL_checkudata(L, 2, "kaleidoscope.quat"));
	F32 t = static_cast<F32>(luaL_checknumber(L, 3));
	quat* n = newquat(L);
	*n = kaleidoscope::math::lerp(*v1, *v2, t);
	return 1;
}


static int lua_newquat(lua_State* L)
{

	I32 numArgs = lua_gettop(L);
	if (numArgs == 0)
	{
		quat* n = newquat(L);
		*n = quat();
		return 1;
	}
	else if (numArgs == 1)
	{
		quat* cq = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
		quat* n = newquat(L);
		*n = *cq;
		return 1;
	}
	else if (numArgs == 4)
	{
		F32 w = static_cast<F32>(luaL_checknumber(L, 1));
		F32 x = static_cast<F32>(luaL_checknumber(L, 2));
		F32 y = static_cast<F32>(luaL_checknumber(L, 3));
		F32 z = static_cast<F32>(luaL_checknumber(L, 4));
		quat* n = newquat(L);
		*n = quat(w, x, y, z);
		return 1;
	}

	return luaL_error(L, "incorrect constructor: try default, 1 quat, or 4 numbers");
}


static int lua_quatindex(lua_State* L)
{
	quat* q = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	const char * field = luaL_checkstring(L, 2);

	if ((strcmp(field, "x") == 0))
	{
		lua_pushnumber(L, q->x);
		return 1;
	}
	else if ((strcmp(field, "y") == 0))
	{
		lua_pushnumber(L, q->y);
		return 1;
	}
	else if ((strcmp(field, "z") == 0))
	{
		lua_pushnumber(L, q->z);
		return 1;
	}
	else if ((strcmp(field, "w") == 0))
	{
		lua_pushnumber(L, q->w);
		return 1;
	}

	return luaL_argerror(L, 2, "not a valid argument");
}


static int lua_quatsetindex(lua_State* L)
{
	quat* q = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	const char * field = luaL_checkstring(L, 2);
	F32 val = static_cast<F32>(luaL_checknumber(L, 3));

	if ((strcmp(field, "x") == 0))
	{
		q->x = val;
	}
	else if ((strcmp(field, "y") == 0))
	{
		q->y = val;
	}
	else if ((strcmp(field, "z") == 0))
	{
		q->z = val;
	}
	else if ((strcmp(field, "w") == 0))
	{
		q->w = val;
	}

	return 0;
}


static int lua_addquatandquat(lua_State* L)
{
	quat* lhs = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	quat* rhs = static_cast<quat*>(luaL_checkudata(L, 2, quatTypeName));

	quat* n = newquat(L);

	*n = (*lhs) + (*rhs);
	return 1;
}


static int lua_quatadd(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_addquatandquat(L);
	}

	return luaL_error(L, "general add error.");
}


static int lua_mulquatandfloat(lua_State* L)
{
	quat* lhs = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	quat* n = newquat(L);

	*n = (*lhs) * rhs;
	return 1;
}


static int lua_mulfloatandquat(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	quat* rhs = static_cast<quat*>(luaL_checkudata(L, 2, quatTypeName));

	quat* n = newquat(L);

	*n = lhs * (*rhs);
	return 1;
}


static int lua_mulquatandquat(lua_State* L)
{
	quat* lhs = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	quat* rhs = static_cast<quat*>(luaL_checkudata(L, 2, quatTypeName));

	quat* n = newquat(L);

	*n = (*lhs) * (*rhs);
	return 1;
}


static int lua_mulquatandvec4(lua_State* L)
{
	quat* lhs = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, vec4TypeName));

	vec4* n = newvec4(L);

	*n = (*lhs) * (*rhs);
	return 1;
}


static int lua_quatmul(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_mulquatandfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_mulfloatandquat(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && ((rhsType == LUA_TUSERDATA) && (luaL_testudata(L, 2, quatTypeName))))
	{
		return lua_mulquatandquat(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && ((rhsType == LUA_TUSERDATA) && (luaL_testudata(L, 2, vec4TypeName))))
	{
		return lua_mulquatandvec4(L);
	}


	return luaL_error(L, "general mul error.");
}


static int lua_divquatandfloat(lua_State* L)
{
	quat* lhs = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	quat* n = newquat(L);

	*n = (*lhs) / rhs;
	return 1;
}


static int lua_quatdiv(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_divquatandfloat(L);
	}

	return luaL_error(L, "general sub error.");
}


static int lua_quatunm(lua_State* L)
{
	quat* v = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	quat* n = newquat(L);
	*n = -(*v);
	return 1;
}


static int lua_quatpow(lua_State* L)
{
	quat* base = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	F32 exp = static_cast<F32>(luaL_checknumber(L, 2));

	quat* v = newquat(L);
	*v = kaleidoscope::math::pow(*base, exp);
	return 1;
}


static int lua_quatequal(lua_State* L)
{
	quat* lhs = static_cast<quat*>(luaL_checkudata(L, 1, quatTypeName));
	quat* rhs = static_cast<quat*>(luaL_checkudata(L, 2, quatTypeName));
	lua_pushboolean(L, ((*lhs) == (*rhs)));
	return 1;
}


static const struct luaL_Reg quat_sf[] =
{
	{ "new", lua_newquat },
	{ "cross", lua_quatcross },
	{ "normalize", lua_quatnormalize },
	{ "slerp", lua_quatslerp },
	{ "lerp", lua_quatlerp },
	{ NULL, NULL }
};


static const struct luaL_Reg quat_mf[] =
{
	{ "__index", lua_quatindex },
	{ "__newindex", lua_quatsetindex},
	{ "__add", lua_quatadd },
	{ "__mul", lua_quatmul },
	{ "__div", lua_quatdiv },
	{ "__unm", lua_quatunm },
	{ "__pow", lua_quatpow },
	{ "__eq", lua_quatequal },
	{ NULL, NULL }
};


int kaleidoscope::luaopen_quat(lua_State* L)
{
	luaL_newmetatable(L, quatTypeName);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, quat_mf, 0);

	luaL_newlib(L, quat_sf);
	lua_setglobal(L, "quat");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}