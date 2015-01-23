#include <LuaLibs/Vec4/Vec4LibLua.h>

#include <Utility/Typedefs.h>
#include <Math/Math.h>

#include <cstring>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static const char * typeName = "kaleidoscope.vec4";
static const char * mat4TypeName = "kaleidoscope.mat4";
static const char * quatTypeName = "kaleidoscope.quat";

using kaleidoscope::math::vec4;
static vec4* newvec4(lua_State* L)
{
	vec4* n = static_cast<vec4*>(lua_newuserdata(L, sizeof(vec4)));
	luaL_getmetatable(L, typeName);
	lua_setmetatable(L, -2);
	return n;
}


using kaleidoscope::math::mat4;
static mat4* newmat4(lua_State* L)
{
	mat4* n = static_cast<mat4*>(lua_newuserdata(L, sizeof(mat4)));
	luaL_getmetatable(L, mat4TypeName);
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


static int lua_vec4dotvec4andvec4(lua_State* L)
{
	vec4* v1 = static_cast<vec4*>(luaL_checkudata(L, 1, "kaleidoscope.vec4"));
	vec4* v2 = static_cast<vec4*>(luaL_checkudata(L, 2, "kaleidoscope.vec4"));
	lua_pushnumber(L, kaleidoscope::math::dot(*v1, *v2));
	return 1;
}


static int lua_vec4normalize(lua_State* L)
{
	vec4* v1 = static_cast<vec4*>(luaL_checkudata(L, 1, "kaleidoscope.vec4"));
	vec4* n = newvec4(L);
	*n = kaleidoscope::math::normalize(*v1);
	return 1;
}


static int lua_vec4abs(lua_State* L)
{
	vec4* v1 = static_cast<vec4*>(luaL_checkudata(L, 1, "kaleidoscope.vec4"));
	vec4* n = newvec4(L);
	*n = kaleidoscope::math::abs(*v1);
	return 1;
}


static int lua_vec4defaultconstructor(lua_State* L, kaleidoscope::math::vec4* v)
{
	*v = kaleidoscope::math::vec4();
	return 1;
}


static int lua_vec4copyconstructor(lua_State* L, kaleidoscope::math::vec4* v)
{
	kaleidoscope::math::vec4* c = static_cast<kaleidoscope::math::vec4*>(luaL_checkudata(L, 1, "kaleidoscope.vec4"));
	*v = *c;
	return 1;
}


static int lua_vec4vec3constructor(lua_State* L, kaleidoscope::math::vec4* v)
{
	kaleidoscope::math::vec3* c = static_cast<kaleidoscope::math::vec3*>(luaL_checkudata(L, 1, "kaleidoscope.vec3"));
	F32 w = static_cast<F32>(luaL_checknumber(L, 2));

	*v = vec4(*c, w);
	return 1;
}


static int lua_vec4fullconstructor(lua_State* L, kaleidoscope::math::vec4* v)
{
	F32 x = static_cast<F32>(luaL_checknumber(L, 1));
	F32 y = static_cast<F32>(luaL_checknumber(L, 2));
	F32 z = static_cast<F32>(luaL_checknumber(L, 3));
	F32 w = static_cast<F32>(luaL_checknumber(L, 4));
	*v = kaleidoscope::math::vec4(x, y, z, w);
	return 1;
}


static int lua_newvec4(lua_State* L)
{
	kaleidoscope::math::vec4* v;

	I32 numArgs = lua_gettop(L);
	if (numArgs == 0)
	{
		v = static_cast<kaleidoscope::math::vec4*>(lua_newuserdata(L, sizeof(kaleidoscope::math::vec4)));
		lua_vec4defaultconstructor(L, v);
	}
	else if (numArgs == 1)
	{
		v = static_cast<kaleidoscope::math::vec4*>(lua_newuserdata(L, sizeof(kaleidoscope::math::vec4)));
		lua_vec4copyconstructor(L, v);
	}
	else if (numArgs == 2)
	{
		v = static_cast<kaleidoscope::math::vec4*>(lua_newuserdata(L, sizeof(kaleidoscope::math::vec4)));
		lua_vec4vec3constructor(L, v);
	}
	else if (numArgs == 4)
	{
		v = static_cast<kaleidoscope::math::vec4*>(lua_newuserdata(L, sizeof(kaleidoscope::math::vec4)));
		lua_vec4fullconstructor(L, v);
	}

	luaL_getmetatable(L, "kaleidoscope.vec4");
	lua_setmetatable(L, -2);
	return 1;
}


static int lua_vec4getx(lua_State* L)
{
	kaleidoscope::math::vec4* v = static_cast<kaleidoscope::math::vec4*>(luaL_checkudata(L, 1, "kaleidoscope.vec4"));
	lua_pushnumber(L, v->x);
	return 1;
}


static int lua_vec4gety(lua_State* L)
{
	kaleidoscope::math::vec4* v = static_cast<kaleidoscope::math::vec4*>(luaL_checkudata(L, 1, "kaleidoscope.vec4"));
	lua_pushnumber(L, v->y);
	return 1;
}


static int lua_vec4getz(lua_State* L)
{
	kaleidoscope::math::vec4* v = static_cast<kaleidoscope::math::vec4*>(luaL_checkudata(L, 1, "kaleidoscope.vec4"));
	lua_pushnumber(L, v->z);
	return 1;
}


static int lua_vec4getw(lua_State* L)
{
	kaleidoscope::math::vec4* v = static_cast<kaleidoscope::math::vec4*>(luaL_checkudata(L, 1, "kaleidoscope.vec4"));
	lua_pushnumber(L, v->w);
	return 1;
}


static int lua_vec4index(lua_State* L)
{
	vec4* v = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	const char * field = luaL_checkstring(L, 2);

	if ((strcmp(field, "x") == 0) || (strcmp(field, "r") == 0))
	{
		lua_pushnumber(L, v->x);
		return 1;
	}
	else if ((strcmp(field, "y") == 0) || (strcmp(field, "g") == 0))
	{
		lua_pushnumber(L, v->y);
		return 1;
	}
	else if ((strcmp(field, "z") == 0) || (strcmp(field, "b") == 0))
	{
		lua_pushnumber(L, v->z);
		return 1;
	}
	else if ((strcmp(field, "w") == 0) || (strcmp(field, "a") == 0))
	{
		lua_pushnumber(L, v->w);
		return 1;
	}

	return luaL_argerror(L, 2, "not a valid argument");
}


static int lua_vec4setindex(lua_State* L)
{
	vec4* v = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	const char * field = luaL_checkstring(L, 2);
	F32 val = static_cast<F32>(luaL_checknumber(L, 3));

	if ((strcmp(field, "x") == 0) || (strcmp(field, "r")))
	{
		v->x = val;
	}
	else if ((strcmp(field, "y") == 0) || (strcmp(field, "g")))
	{
		v->y = val;
	}
	else if ((strcmp(field, "z") == 0) || (strcmp(field, "b")))
	{
		v->z = val;
	}
	else if ((strcmp(field, "w") == 0) || (strcmp(field, "a")))
	{
		v->w = val;
	}

	return 0;
}


static int lua_addvec4andfloat(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	vec4* n = newvec4(L);

	*n = (*lhs) + rhs;
	return 1;
}


static int lua_addfloatandvec4(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, typeName));

	vec4* n = newvec4(L);

	*n = lhs + (*rhs);
	return 1;
}


static int lua_addvec4andvec4(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, typeName));

	vec4* n = newvec4(L);

	*n = (*lhs) + (*rhs);
	return 1;
}


static int lua_vec4add(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_addvec4andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_addfloatandvec4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_addvec4andvec4(L);
	}

	return 0;
}


static int lua_subvec4andfloat(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	vec4* n = newvec4(L);

	*n = (*lhs) - rhs;
	return 1;
}


static int lua_subfloatandvec4(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, typeName));

	vec4* n = newvec4(L);

	*n = lhs - (*rhs);
	return 1;
}


static int lua_subvec4andvec4(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, typeName));

	vec4* n = newvec4(L);

	*n = (*lhs) - (*rhs);
	return 1;
}


static int lua_vec4sub(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_subvec4andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_subfloatandvec4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_subvec4andvec4(L);
	}

	return 0;
}


static int lua_mulvec4andfloat(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	vec4* n = newvec4(L);

	*n = (*lhs) * rhs;
	return 1;
}


static int lua_mulfloatandvec4(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, typeName));

	vec4* n = newvec4(L);

	*n = lhs * (*rhs);
	return 1;
}


static int lua_mulvec4andvec4(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, typeName));

	vec4* n = newvec4(L);

	*n = (*lhs) * (*rhs);
	return 1;
}


static int lua_mulvec4andmat4(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));

	vec4* n = newvec4(L);

	*n = (*lhs) * (*rhs);
	return 1;
}


static int lua_mulvec4andquat(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	quat* rhs = static_cast<quat*>(luaL_checkudata(L, 2, quatTypeName));

	vec4* n = newvec4(L);

	*n = (*lhs) * (*rhs);
	return 1;
}


static int lua_vec4mul(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_mulvec4andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_mulfloatandvec4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && ((rhsType == LUA_TUSERDATA) && (luaL_testudata(L, 2, typeName))))
	{
		return lua_mulvec4andvec4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && ((rhsType == LUA_TUSERDATA) && (luaL_testudata(L, 2, mat4TypeName))))
	{
		return lua_mulvec4andmat4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && ((rhsType == LUA_TUSERDATA) && (luaL_testudata(L, 2, quatTypeName))))
	{
		return lua_mulvec4andquat(L);
	}

	return 0;
}


static int lua_divvec4andfloat(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	vec4* n = newvec4(L);

	*n = (*lhs) / rhs;
	return 1;
}


static int lua_divfloatandvec4(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, typeName));

	vec4* n = newvec4(L);

	*n = lhs / (*rhs);
	return 1;
}


static int lua_divvec4andvec4(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, typeName));

	vec4* n = newvec4(L);

	*n = (*lhs) / (*rhs);
	return 1;
}


static int lua_vec4div(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_divvec4andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_divfloatandvec4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_divvec4andvec4(L);
	}

	return 0;
}


static int lua_vec4unm(lua_State* L)
{
	vec4* v = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	vec4* n = newvec4(L);
	*n = -(*v);
	return 1;
}


static int lua_vec4pow(lua_State* L)
{
	kaleidoscope::math::vec4* base = static_cast<kaleidoscope::math::vec4*>(luaL_checkudata(L, 1, "kaleidoscope.vec4"));
	kaleidoscope::math::vec4* exp = static_cast<kaleidoscope::math::vec4*>(luaL_checkudata(L, 2, "kaleidoscope.vec4"));

	kaleidoscope::math::vec4* v = newvec4(L);
	*v = kaleidoscope::math::pow(*base, *exp);
	return 1;
}


static int lua_vec4equal(lua_State* L)
{
	vec4* lhs = static_cast<vec4*>(luaL_checkudata(L, 1, typeName));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, typeName));
	lua_pushboolean(L, ((*lhs) == (*rhs)));
	return 1;
}


static const struct luaL_Reg vec4_sf[] = 
{
	{ "new", lua_newvec4 },
	{ "dot", lua_vec4dotvec4andvec4 },
	{ "normalize", lua_vec4normalize },
	{ "abs", lua_vec4abs },
	{ NULL, NULL }
};


static const struct luaL_Reg vec4_mf[] = 
{
	{ "__index", lua_vec4index },
	{ "__newindex", lua_vec4setindex },
	{ "__add", lua_vec4add },
	{ "__sub", lua_vec4sub },
	{ "__mul", lua_vec4mul },
	{ "__div", lua_vec4div },
	{ "__unm", lua_vec4unm },
	{ "__pow", lua_vec4pow },
	{ "__eq", lua_vec4equal },
	{ NULL, NULL }
};


int kaleidoscope::luaopen_vec4(lua_State* L)
{
	luaL_newmetatable(L, "kaleidoscope.vec4");

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, vec4_mf, 0);

	luaL_newlib(L, vec4_sf);
	lua_setglobal(L, "vec4");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}