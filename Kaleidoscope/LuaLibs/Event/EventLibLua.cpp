#include <LuaLibs/Event/EventLibLua.h>

#include <Event/Event.h>

#include <Utility/StringID/StringId.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static const char * eventTypeName = "kaleidoscope.event";
static const char * vec3TypeName = "kaleidoscope.vec3";
static const char * vec4TypeName = "kaleidoscope.vec4";
static const char * quatTypeName = "kaleidoscope.quat";

using kaleidoscope::Event;
using kaleidoscope::math::vec3;
using kaleidoscope::math::vec4;
using kaleidoscope::math::quat;

static Event* newevent(lua_State* L)
{
	Event* n = static_cast<Event*>(lua_newuserdata(L, sizeof(Event)));
	luaL_getmetatable(L, eventTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static vec3* newvec3(lua_State* L)
{
	vec3* n = static_cast<vec3*>(lua_newuserdata(L, sizeof(vec3)));
	luaL_getmetatable(L, vec3TypeName);
	lua_setmetatable(L, -2);
	return n;
}

static vec4* newvec4(lua_State* L)
{
	vec4* n = static_cast<vec4*>(lua_newuserdata(L, sizeof(vec4)));
	luaL_getmetatable(L, vec4TypeName);
	lua_setmetatable(L, -2);
	return n;
}

static quat* newquat(lua_State* L)
{
	quat* n = static_cast<quat*>(lua_newuserdata(L, sizeof(quat)));
	luaL_getmetatable(L, quatTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static int lua_eventnew(lua_State* L)
{
	Event* e = newevent(L);
	*e = Event();
	return 1;
}

static int lua_eventseteventtype(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	kaleidoscope::StringID type = static_cast<kaleidoscope::StringID>(luaL_checkunsigned(L, 2));
	e->setEventType(type);

	return 0;
}

static int lua_eventgeteventtype(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	lua_pushnumber(L, e->getEventType());
	return 1;
}

static int lua_eventmaxsize(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	lua_pushnumber(L, e->maxSize());
	return 1;
}

static int lua_eventaddnumber(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	U32 i = static_cast<U32>(luaL_checkunsigned(L, 2));
	F32 val = static_cast<F32>(luaL_checknumber(L, 3));
	luaL_argcheck(L, (i < e->maxSize() && i >= 0), 1, "index is out of range");
	e->addF32(i, val);
	return 0;
}

static int lua_eventaddvec3(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	U32 i = static_cast<U32>(luaL_checkunsigned(L, 2));
	vec3* val = static_cast<vec3*>(luaL_checkudata(L, 3, vec3TypeName));
	luaL_argcheck(L, (i < e->maxSize() && i >= 0), 1, "index is out of range");
	e->addVec3(i, *val);
	return 0;
}

static int lua_eventaddvec4(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	U32 i = static_cast<U32>(luaL_checkunsigned(L, 2));
	vec4* val = static_cast<vec4*>(luaL_checkudata(L, 3, vec4TypeName));
	luaL_argcheck(L, (i < e->maxSize() && i >= 0), 1, "index is out of range");
	e->addVec4(i, *val);
	return 0;
}

static int lua_eventaddquat(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	U32 i = static_cast<U32>(luaL_checkunsigned(L, 2));
	quat* val = static_cast<quat*>(luaL_checkudata(L, 3, quatTypeName));
	luaL_argcheck(L, (i < e->maxSize() && i >= 0), 1, "index is out of range");
	e->addQuat(i, *val);
	return 0;
}

static int lua_eventgetnumber(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	U32 i = static_cast<U32>(luaL_checkunsigned(L, 2));
	luaL_argcheck(L, (i < e->maxSize() && i >= 0), 1, "index is out of range");

	lua_pushnumber(L, e->getF32(i));

	return 1;
}

static int lua_eventgetvec3(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	U32 i = static_cast<U32>(luaL_checkunsigned(L, 2));
	luaL_argcheck(L, (i < e->maxSize() && i >= 0), 1, "index is out of range");

	vec3* v = newvec3(L);
	*v = e->getVec3(i);

	return 1;
}

static int lua_eventgetvec4(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	U32 i = static_cast<U32>(luaL_checkunsigned(L, 2));
	luaL_argcheck(L, (i < e->maxSize() && i >= 0), 1, "index is out of range");

	vec4* v = newvec4(L);
	*v = e->getVec4(i);

	return 1;
}

static int lua_eventgetquat(lua_State* L)
{
	Event* e = static_cast<Event*>(luaL_checkudata(L, 1, eventTypeName));
	U32 i = static_cast<U32>(luaL_checkunsigned(L, 2));
	luaL_argcheck(L, (i < e->maxSize() && i >= 0), 1, "index is out of range");

	quat* v = newquat(L);
	*v = e->getQuat(i);

	return 1;
}


static const struct luaL_Reg event_sf[] =
{
	{ "new", lua_eventnew },
	{ NULL, NULL }
};

static const struct luaL_Reg event_mf[] = 
{
	{ "setEventType", lua_eventseteventtype },
	{ "getEventType", lua_eventgeteventtype },
	{ "maxSize", lua_eventmaxsize },

	{ "addNumber", lua_eventaddnumber },
	{ "addVec3", lua_eventaddvec3 },
	{ "addVec4", lua_eventaddvec4 },
	{ "addQuat", lua_eventaddquat },

	{ "getNumber", lua_eventgetnumber },
	{ "getVec3", lua_eventgetvec3 },
	{ "getVec4", lua_eventgetvec4 },
	{ "getQuat", lua_eventgetquat },

	{ NULL, NULL }
};

int kaleidoscope::luaopen_event(lua_State* L)
{
	luaL_newmetatable(L, eventTypeName);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, event_mf, 0);

	luaL_newlib(L, event_sf);
	lua_setglobal(L, "kEvent");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}