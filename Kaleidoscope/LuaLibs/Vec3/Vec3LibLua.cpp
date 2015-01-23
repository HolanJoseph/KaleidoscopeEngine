#include <LuaLibs/Vec3/Vec3LibLua.h>

#include <Utility/Typedefs.h>

#include <Math/Math.h>

#include <cstring>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static const char * typeName = "kaleidoscope.vec3";

using kaleidoscope::math::vec3;
static vec3* newvec3(lua_State* L)
{
	vec3* n = static_cast<vec3*>(lua_newuserdata(L, sizeof(vec3)));
	luaL_getmetatable(L, typeName);
	lua_setmetatable(L, -2);
	return n;
}


static int lua_vec3crossvec3andvec3(lua_State* L)
{
	kaleidoscope::math::vec3* v1 = static_cast<kaleidoscope::math::vec3*>(luaL_checkudata(L, 1, "kaleidoscope.vec3"));
	kaleidoscope::math::vec3* v2 = static_cast<kaleidoscope::math::vec3*>(luaL_checkudata(L, 2, "kaleidoscope.vec3"));
	vec3* n = newvec3(L);
	*n = kaleidoscope::math::cross(*v1, *v2);
	return 1;
}

using kaleidoscope::math::quat;
static int lua_vec3crossvec3andquat(lua_State* L)
{
	vec3* v1 = static_cast<vec3*>(luaL_checkudata(L, 1, "kaleidoscope.vec3"));
	quat* v2 = static_cast<quat*>(luaL_checkudata(L, 2, "kaleidoscope.quat"));
	vec3* n = newvec3(L);
	*n = kaleidoscope::math::cross(*v1, *v2);
	return 1;
}

static int lua_vec3cross(lua_State* L)
{
	if ((luaL_testudata(L, 1, typeName)) && (luaL_testudata(L, 2, typeName)))
	{
		return lua_vec3crossvec3andvec3(L);
	}
	else if ((luaL_testudata(L, 1, typeName)) && (luaL_testudata(L, 2, "kaleidoscope.quat")))
	{
		return lua_vec3crossvec3andquat(L);
	}

	return luaL_error(L, "incorrect args");
}


static int lua_vec3dotvec3andvec3(lua_State* L)
{
	vec3* v1 = static_cast<vec3*>(luaL_checkudata(L, 1, "kaleidoscope.vec3"));
	vec3* v2 = static_cast<vec3*>(luaL_checkudata(L, 2, "kaleidoscope.vec3"));
	lua_pushnumber(L, kaleidoscope::math::dot(*v1, *v2));
	return 1;
}


static int lua_vec3normalize(lua_State* L)
{
	vec3* v1 = static_cast<vec3*>(luaL_checkudata(L, 1, "kaleidoscope.vec3"));
	vec3* n = newvec3(L);

	//gLogManager.log("v1 = (%f, %f, %f)", v1->x, v1->y, v1->z);

	*n = kaleidoscope::math::normalize(*v1);
	return 1;
}


static int lua_vec3abs(lua_State* L)
{
	vec3* v1 = static_cast<vec3*>(luaL_checkudata(L, 1, "kaleidoscope.vec3"));
	vec3* n = newvec3(L);
	*n = kaleidoscope::math::abs(*v1);
	return 1;
}


static int lua_vec3defaultconstructor(lua_State* L, kaleidoscope::math::vec3* v)
{
	*v = kaleidoscope::math::vec3();
	return 1;
}


static int lua_vec3copyconstructor(lua_State* L, kaleidoscope::math::vec3* v)
{
	kaleidoscope::math::vec3* c = static_cast<kaleidoscope::math::vec3*>(luaL_checkudata(L, 1, "kaleidoscope.vec3"));
	*v = *c;
	return 1;
}


static int lua_vec3fullconstructor(lua_State* L, kaleidoscope::math::vec3* v)
{
	F32 x = static_cast<F32>(luaL_checknumber(L, 1));
	F32 y = static_cast<F32>(luaL_checknumber(L, 2));
	F32 z = static_cast<F32>(luaL_checknumber(L, 3));
	*v = kaleidoscope::math::vec3(x, y, z);
	return 1;
}


static int lua_newvec3(lua_State* L)
{
	kaleidoscope::math::vec3* v;

	I32 numArgs = lua_gettop(L);
	if (numArgs == 0)
	{
		v = static_cast<kaleidoscope::math::vec3*>(lua_newuserdata(L, sizeof(kaleidoscope::math::vec3)));
		lua_vec3defaultconstructor(L, v);
	}
	else if (numArgs == 1)
	{
		v = static_cast<kaleidoscope::math::vec3*>(lua_newuserdata(L, sizeof(kaleidoscope::math::vec3)));
		lua_vec3copyconstructor(L, v);
	}
	else if (numArgs == 3)
	{
		v = static_cast<kaleidoscope::math::vec3*>(lua_newuserdata(L, sizeof(kaleidoscope::math::vec3)));
		lua_vec3fullconstructor(L, v);
	}

	luaL_getmetatable(L, "kaleidoscope.vec3");
	lua_setmetatable(L, -2);
	return 1;
}


static int lua_vec3index(lua_State* L)
{
	vec3* v = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
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

	return luaL_argerror(L, 2, "not a valid argument");
}


static int lua_vec3setindex(lua_State* L)
{
	vec3* v = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	const char * field = luaL_checkstring(L, 2);
	F32 val = static_cast<F32>(luaL_checknumber(L, 3));

	if ((strcmp(field, "x") == 0) || (strcmp(field, "r") == 0))
	{
		v->x = val;
	}
	else if ((strcmp(field, "y") == 0) || (strcmp(field, "g") == 0))
	{
		v->y = val;
	}
	else if ((strcmp(field, "z") == 0) || (strcmp(field, "b") == 0))
	{
		v->z = val;
	}

	return 0;
}


static int lua_addvec3andfloat(lua_State* L)
{
	vec3* lhs = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	vec3* n = newvec3(L);

	*n = (*lhs) + rhs;
	return 1;
}


static int lua_addfloatandvec3(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	vec3* rhs = static_cast<vec3*>(luaL_checkudata(L, 2, typeName));

	vec3* n = newvec3(L);

	*n = lhs + (*rhs);
	return 1;
}


static int lua_addvec3andvec3(lua_State* L)
{
	vec3* lhs = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	vec3* rhs = static_cast<vec3*>(luaL_checkudata(L, 2, typeName));

	vec3* n = newvec3(L);

	*n = (*lhs) + (*rhs);
	return 1;
}


static int lua_vec3add(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_addvec3andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_addfloatandvec3(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_addvec3andvec3(L);
	}

	return 0;
}


static int lua_subvec3andfloat(lua_State* L)
{
	vec3* lhs = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	vec3* n = newvec3(L);

	*n = (*lhs) - rhs;
	return 1;
}


static int lua_subfloatandvec3(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	vec3* rhs = static_cast<vec3*>(luaL_checkudata(L, 2, typeName));

	vec3* n = newvec3(L);

	*n = lhs - (*rhs);
	return 1;
}


static int lua_subvec3andvec3(lua_State* L)
{
	vec3* lhs = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	vec3* rhs = static_cast<vec3*>(luaL_checkudata(L, 2, typeName));

	vec3* n = newvec3(L);

	*n = (*lhs) - (*rhs);
	return 1;
}


static int lua_vec3sub(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_subvec3andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_subfloatandvec3(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_subvec3andvec3(L);
	}

	return 0;
}


static int lua_mulvec3andfloat(lua_State* L)
{
	vec3* lhs = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	vec3* n = newvec3(L);

	*n = (*lhs) * rhs;
	return 1;
}


static int lua_mulfloatandvec3(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	vec3* rhs = static_cast<vec3*>(luaL_checkudata(L, 2, typeName));

	vec3* n = newvec3(L);

	*n = lhs * (*rhs);
	return 1;
}


static int lua_mulvec3andvec3(lua_State* L)
{
	vec3* lhs = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	vec3* rhs = static_cast<vec3*>(luaL_checkudata(L, 2, typeName));

	vec3* n = newvec3(L);

	*n = (*lhs) * (*rhs);
	return 1;
}


static int lua_vec3mul(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_mulvec3andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_mulfloatandvec3(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && ((rhsType == LUA_TUSERDATA) && (luaL_testudata(L, 2, typeName))))
	{
		return lua_mulvec3andvec3(L);
	}

	return 0;
}


static int lua_divvec3andfloat(lua_State* L)
{
	vec3* lhs = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	F32 rhs = static_cast<F32>(luaL_checknumber(L, 2));

	vec3* n = newvec3(L);

	*n = (*lhs) / rhs;
	return 1;
}


static int lua_divfloatandvec3(lua_State* L)
{
	F32 lhs = static_cast<F32>(luaL_checknumber(L, 1));
	vec3* rhs = static_cast<vec3*>(luaL_checkudata(L, 2, typeName));

	vec3* n = newvec3(L);

	*n = lhs / (*rhs);
	return 1;
}


static int lua_divvec3andvec3(lua_State* L)
{
	vec3* lhs = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	vec3* rhs = static_cast<vec3*>(luaL_checkudata(L, 2, typeName));

	vec3* n = newvec3(L);

	*n = (*lhs) / (*rhs);
	return 1;
}


static int lua_vec3div(lua_State* L)
{
	int lhsType = lua_type(L, 1);
	int rhsType = lua_type(L, 2);

	if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TNUMBER))
	{
		return lua_divvec3andfloat(L);
	}
	else if ((lhsType == LUA_TNUMBER) && (rhsType == LUA_TUSERDATA))
	{
		return lua_divfloatandvec3(L);
	}
	else if ((lhsType == LUA_TUSERDATA) && (rhsType == LUA_TUSERDATA))
	{
		return lua_divvec3andvec3(L);
	}

	return 0;
}


static int lua_vec3unm(lua_State* L)
{
	vec3* v = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	vec3* n = newvec3(L);
	*n = -(*v);
	return 1;
}


static int lua_vec3pow(lua_State* L)
{
	kaleidoscope::math::vec3* base = static_cast<kaleidoscope::math::vec3*>(luaL_checkudata(L, 1, "kaleidoscope.vec3"));
	kaleidoscope::math::vec3* exp = static_cast<kaleidoscope::math::vec3*>(luaL_checkudata(L, 2, "kaleidoscope.vec3"));

	kaleidoscope::math::vec3* v = newvec3(L);
	*v = kaleidoscope::math::pow(*base, *exp);
	return 1;
}


static int lua_vec3equal(lua_State* L)
{
	vec3* lhs = static_cast<vec3*>(luaL_checkudata(L, 1, typeName));
	vec3* rhs = static_cast<vec3*>(luaL_checkudata(L, 2, typeName));
	lua_pushboolean(L, ((*lhs) == (*rhs)));
	return 1;
}


static const struct luaL_Reg vec3_sf[] =
{
	{ "new", lua_newvec3 },
	{ "cross", lua_vec3cross },
	{ "dot", lua_vec3dotvec3andvec3 },
	{ "normalize", lua_vec3normalize },
	{ "abs", lua_vec3abs },
	{ NULL, NULL }
};


static const struct luaL_Reg vec3_mf[] =
{
	{ "__index", lua_vec3index },
	{ "__newindex", lua_vec3setindex },
	{ "__add", lua_vec3add },
	{ "__sub", lua_vec3sub },
	{ "__mul", lua_vec3mul },
	{ "__div", lua_vec3div },
	{ "__unm", lua_vec3unm },
	{ "__pow", lua_vec3pow },
	{ "__eq", lua_vec3equal },
	{ NULL, NULL }
};


int kaleidoscope::luaopen_vec3(lua_State* L)
{
	luaL_newmetatable(L, "kaleidoscope.vec3");

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, vec3_mf, 0);

	luaL_newlib(L, vec3_sf);
	lua_setglobal(L, "vec3");

	lua_pop(L, 1); // pop the meta table.
	return 0;
}