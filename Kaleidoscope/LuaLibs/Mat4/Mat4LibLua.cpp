#include <LuaLibs/Mat4/Mat4LibLua.h>

#include <Utility/Typedefs.h>
#include <Math/Math.h>

#include <cstring>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static const char * mat4TypeName = "kaleidoscope.mat4";
static const char * vec4TypeName = "kaleidoscope.vec4";

using kaleidoscope::math::vec4;
static vec4* newvec4(lua_State* L)
{
	vec4* n = static_cast<vec4*>(lua_newuserdata(L, sizeof(vec4)));
	luaL_getmetatable(L, vec4TypeName);
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


static int lua_mat4get(lua_State* L)
{
	mat4* m = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	I32 c = luaL_checkint(L, 2);
	I32 r = luaL_checkint(L, 3);
	luaL_argcheck(L, ((c >= 0) && (c < 4)), 2, "column value is out of range");
	luaL_argcheck(L, ((r >= 0) && (r < 4)), 3, "row value is out of range");
	lua_pushnumber(L, (*m)[c][r]);
	return 1;
}


static int lua_mat4set(lua_State* L)
{
	mat4* m = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	I32 c = luaL_checkint(L, 2);
	I32 r = luaL_checkint(L, 3);
	luaL_argcheck(L, ((c >= 0) && (c < 4)), 2, "column value is out of range");
	luaL_argcheck(L, ((r >= 0) && (r < 4)), 3, "row value is out of range");
	F32 value = static_cast<F32>(luaL_checknumber(L, 4));
	(*m)[c][r] = value;
	return 0;
}


static int lua_newmat4(lua_State* L)
{

	I32 numArgs = lua_gettop(L);
	if (numArgs == 0)
	{
		mat4* n = newmat4(L);
		*n = mat4();
		return 1;
	}
	else if (numArgs == 1)
	{
		mat4* cm = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
		mat4* n = newmat4(L);
		*n = *cm;
		return 1;
	}
	else if (numArgs == 4)
	{
		vec4* c1 = static_cast<vec4*>(luaL_checkudata(L, 1, vec4TypeName));
		vec4* c2 = static_cast<vec4*>(luaL_checkudata(L, 2, vec4TypeName));
		vec4* c3 = static_cast<vec4*>(luaL_checkudata(L, 3, vec4TypeName));
		vec4* c4 = static_cast<vec4*>(luaL_checkudata(L, 4, vec4TypeName));
		mat4* n = newmat4(L);
		*n = mat4(*c1, *c2, *c3, *c4);
		return 1;
	}
	else if (numArgs == 16)
	{
		F32 f1  = static_cast<F32>(luaL_checknumber(L, 1));
		F32 f2  = static_cast<F32>(luaL_checknumber(L, 2));
		F32 f3  = static_cast<F32>(luaL_checknumber(L, 3));
		F32 f4  = static_cast<F32>(luaL_checknumber(L, 4));
		F32 f5  = static_cast<F32>(luaL_checknumber(L, 5));
		F32 f6  = static_cast<F32>(luaL_checknumber(L, 6));
		F32 f7  = static_cast<F32>(luaL_checknumber(L, 7));
		F32 f8  = static_cast<F32>(luaL_checknumber(L, 8));
		F32 f9  = static_cast<F32>(luaL_checknumber(L, 9));
		F32 f10 = static_cast<F32>(luaL_checknumber(L, 10));
		F32 f11 = static_cast<F32>(luaL_checknumber(L, 11));
		F32 f12 = static_cast<F32>(luaL_checknumber(L, 12));
		F32 f13 = static_cast<F32>(luaL_checknumber(L, 13));
		F32 f14 = static_cast<F32>(luaL_checknumber(L, 14));
		F32 f15 = static_cast<F32>(luaL_checknumber(L, 15));
		F32 f16 = static_cast<F32>(luaL_checknumber(L, 16));
		mat4* n = newmat4(L);
		*n = mat4(f1, f2, f3, f4,
				  f5, f6, f7, f8,
				  f9, f10, f11, f12,
				  f13, f14, f15, f16);
		return 1;
	}

	return luaL_error(L, "incorrect constructor: try default, 1 mat4, or 16 numbers");
}


static int lua_addmat4andfloat(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	mat4* n = newmat4(L);

	*n = (*lhs) + rhs;
	return 1;
}


static int lua_addfloatandmat4(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));

	mat4* n = newmat4(L);

	*n = lhs + (*rhs);
	return 1;
}


static int lua_addmat4andmat4(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));

	mat4* n = newmat4(L);

	*n = (*lhs) + (*rhs);
	return 1;
}


static int lua_mat4add(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_addmat4andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_addfloatandmat4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_addmat4andmat4(L);
	}

	if ( !( (lhsType == LUA_TNUMBER) || (lhsType == LUA_TUSERDATA) ) )
	{
		return luaL_argerror(L, 1, "incorrect type");
	}
	else if (!((rhsType == LUA_TNUMBER) || (rhsType == LUA_TUSERDATA)))
	{
		return luaL_argerror(L, 2, "incorrect type");
	}
	else
	{
		return luaL_error(L, "general add error.");
	}
}


static int lua_submat4andfloat(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	mat4* n = newmat4(L);

	*n = (*lhs) - rhs;
	return 1;
}


static int lua_subfloatandmat4(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));

	mat4* n = newmat4(L);

	*n = lhs - (*rhs);
	return 1;
}


static int lua_submat4andmat4(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));

	mat4* n = newmat4(L);

	*n = (*lhs) - (*rhs);
	return 1;
}


static int lua_mat4sub(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_submat4andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_subfloatandmat4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_submat4andmat4(L);
	}

	if (!((lhsType == LUA_TNUMBER) || (lhsType == LUA_TUSERDATA)))
	{
		return luaL_argerror(L, 1, "incorrect type");
	}
	else if (!((rhsType == LUA_TNUMBER) || (rhsType == LUA_TUSERDATA)))
	{
		return luaL_argerror(L, 2, "incorrect type");
	}
	else
	{
		return luaL_error(L, "general sub error.");
	}
}


static int lua_mulmat4andfloat(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	mat4* n = newmat4(L);

	*n = (*lhs) * rhs;
	return 1;
}


static int lua_mulfloatandmat4(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));

	mat4* n = newmat4(L);

	*n = lhs * (*rhs);
	return 1;
}


static int lua_mulmat4andmat4(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));

	mat4* n = newmat4(L);

	*n = (*lhs) * (*rhs);
	return 1;
}


static int lua_mulmat4andvec4(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	vec4* rhs = static_cast<vec4*>(luaL_checkudata(L, 2, vec4TypeName));

	vec4* n = newvec4(L);

	*n = (*lhs) * (*rhs);
	return 1;
}


static int lua_mat4mul(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ( (lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_mulmat4andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_mulfloatandmat4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && ((rhsType == LUA_TUSERDATA) && (luaL_testudata(L, 2, mat4TypeName))))
	{
		return lua_mulmat4andmat4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && ((rhsType == LUA_TUSERDATA) && (luaL_testudata(L, 2, vec4TypeName))))
	{
		return lua_mulmat4andvec4(L);
	}

	if (!((lhsType == LUA_TNUMBER) || (lhsType == LUA_TUSERDATA)))
	{
		return luaL_argerror(L, 1, "incorrect type");
	}
	else if (!((rhsType == LUA_TNUMBER) || (rhsType == LUA_TUSERDATA)))
	{
		return luaL_argerror(L, 2, "incorrect type");
	}
	else
	{
		return luaL_error(L, "general mul error.");
	}
}


static int lua_divmat4andfloat(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	mat4* n = newmat4(L);

	*n = (*lhs) / rhs;
	return 1;
}


static int lua_divfloatandmat4(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));

	mat4* n = newmat4(L);

	*n = lhs / (*rhs);
	return 1;
}


static int lua_divmat4andmat4(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));

	mat4* n = newmat4(L);

	*n = (*lhs) / (*rhs);
	return 1;
}


static int lua_mat4div(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_divmat4andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_divfloatandmat4(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_divmat4andmat4(L);
	}

	if (!((lhsType == LUA_TNUMBER) || (lhsType == LUA_TUSERDATA)))
	{
		return luaL_argerror(L, 1, "incorrect type");
	}
	else if (!((rhsType == LUA_TNUMBER) || (rhsType == LUA_TUSERDATA)))
	{
		return luaL_argerror(L, 2, "incorrect type");
	}
	else
	{
		return luaL_error(L, "general sub error.");
	}
}


static int lua_mat4unm(lua_State* L)
{
	mat4* v = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	mat4* n = newmat4(L);
	*n = -(*v);
	return 1;
}


static int lua_mat4equal(lua_State* L)
{
	mat4* lhs = static_cast<mat4*>(luaL_checkudata(L, 1, mat4TypeName));
	mat4* rhs = static_cast<mat4*>(luaL_checkudata(L, 2, mat4TypeName));
	lua_pushboolean(L, ((*lhs) == (*rhs)));
	return 1;
}


static const struct luaL_Reg mat4_sf[] =
{
	{ "new", lua_newmat4 },
	{ NULL, NULL }
};


static const struct luaL_Reg mat4_mf[] =
{
	{ "get", lua_mat4get },
	{ "set", lua_mat4set },
	{ "__add", lua_mat4add },
	{ "__sub", lua_mat4sub },
	{ "__mul", lua_mat4mul },
	{ "__div", lua_mat4div },
	{ "__unm", lua_mat4unm },
	{ "__eq", lua_mat4equal },
	{ NULL, NULL }
};


int kaleidoscope::luaopen_mat4(lua_State* L)
{
	luaL_newmetatable(L, mat4TypeName);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, mat4_mf, 0);

	luaL_newlib(L, mat4_sf);
	lua_setglobal(L, "mat4");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}