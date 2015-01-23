#include <LuaLibs/GameObject/GameObjectHandleLibLua.h>

#include <Math/Math.h>

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <GameObject/GameObjectHandle.h>
#include <Components/LuaScript/LuaScriptHandle.h>
#include <Event/Event.h>
#include <Components/Transform/TransformHandle.h>
#include <GameObject/GameObject.h>
#include <Components/Renderable/Renderable.h>
#include <Components/Renderable/RenderableHandle.h>

#include <list>

#include <LuaLibs/Utility/lua_getters.h>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static const char * transformHandleTypeName = "kaleidoscope.transformHandle";
static const char * luaScriptHandleTypeName = "kaleidoscope.LUAScriptHandle";
static const char * gameObjectHandleTypeName = "kaleidoscope.GameObjectHandle";
static const char * eventTypeName = "kaleidoscope.event";

using kaleidoscope::TransformHandle;
using kaleidoscope::LuaScriptHandle;
using kaleidoscope::GameObjectHandle;
using kaleidoscope::Event;
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

static LuaScriptHandle* newLUAScriptHandle(lua_State* L)
{
	LuaScriptHandle* n = static_cast<LuaScriptHandle*>(lua_newuserdata(L, sizeof(LuaScriptHandle)));
	luaL_getmetatable(L, luaScriptHandleTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static LuaScriptHandle* getLUAScriptHandle(lua_State* L, U32 index)
{
	return static_cast<LuaScriptHandle*>(luaL_checkudata(L, index, luaScriptHandleTypeName));
}

static GameObjectHandle* newGameObjectHandle(lua_State* L)
{
	GameObjectHandle* n = static_cast<GameObjectHandle*>(lua_newuserdata(L, sizeof(GameObjectHandle)));
	luaL_getmetatable(L, gameObjectHandleTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static GameObjectHandle* getGameObjectHandle(lua_State* L, U32 index)
{
	return static_cast<GameObjectHandle*>(luaL_checkudata(L, index, gameObjectHandleTypeName));
}

static Event* newEvent(lua_State* L)
{
	Event* n = static_cast<Event*>(lua_newuserdata(L, sizeof(Event)));
	luaL_getmetatable(L, eventTypeName);
	lua_setmetatable(L, -2);
	return n;
}

static Event* getEvent(lua_State* L, U32 index)
{
	return static_cast<Event*>(luaL_checkudata(L, index, eventTypeName));
}

static kaleidoscope::StringID getstringid(lua_State* L, U32 index)
{
	return static_cast<kaleidoscope::StringID>(luaL_checkunsigned(L, index));
}

static int lua_gocreate(lua_State* L)
{
	I32 numArgs = lua_gettop(L);
	if (numArgs == 1)
	{
		StringID name = getstringid(L, 1);
		GameObjectHandle* n = newGameObjectHandle(L);
		*n = kaleidoscope::GameObject::Create(name);
		return 1;
	}
	else if (numArgs == 2)
	{
		StringID name = getstringid(L, 1);
		bool overwrite;
		if (lua_type(L, 2) == LUA_TBOOLEAN)
		{
			//overwrite = static_cast<bool>(lua_toboolean(L, 2));
			overwrite = (lua_toboolean(L, 2) != 0);
			GameObjectHandle* n = newGameObjectHandle(L);
			*n = kaleidoscope::GameObject::Create(name, overwrite);
			return 1;
		}
		return luaL_error(L, "argument error");
	}


	return luaL_error(L, "argument error");
}

static int lua_godestroy(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	kaleidoscope::GameObject::Destroy(*goh);
	return 0;
}

static int lua_gosendevent(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	Event* e = getEvent(L, 2);
	kaleidoscope::GameObject::SendEvent(*goh, *e);
	return 0;
}

static int lua_gobroadcastevent(lua_State* L)
{
	Event* e = getEvent(L, 1);
	kaleidoscope::GameObject::BroadcastEvent(*e);
	return 0;
}

static int lua_gofindbyname(lua_State* L)
{
	StringID name = getstringid(L, 1);
	GameObjectHandle* n = newGameObjectHandle(L);
	*n = kaleidoscope::GameObject::FindByName(name);
	return 1;
}

static int lua_gofindbytag(lua_State* L)
{
	StringID tag = getstringid(L, 1);
	GameObjectHandle* n = newGameObjectHandle(L);
	*n = kaleidoscope::GameObject::FindByTag(tag);
	return 1;
}

static int lua_go_FindAllWithTag(lua_State* L)
{
	StringID tag = static_cast<StringID>(luaL_checkunsigned(L, 1));
	std::list<GameObjectHandle>* allWTag = kaleidoscope::GameObject::FindAllWithTag(tag);

	lua_createtable(L, allWTag->size(), 0);

	I32 seqNum = 1;
	for (std::list<GameObjectHandle>::iterator goh = allWTag->begin(); goh != allWTag->end(); ++goh)
	{
		lua_pushnumber(L, seqNum++); //push table index
		GameObjectHandle* goh_lua = newudata<GameObjectHandle>(L, gameObjectHandleTypeName); //push copy of childs GOH
		*goh_lua = *goh;
		lua_settable(L, -3);
	}

	delete allWTag;
	return 1;
}

static int lua_go_FindAll(lua_State* L)
{
	std::list<GameObjectHandle>* all = kaleidoscope::GameObject::FindAll();

	lua_createtable(L, all->size(), 0);

	I32 seqNum = 1;
	for (std::list<GameObjectHandle>::iterator goh = all->begin(); goh != all->end(); ++goh)
	{
		lua_pushnumber(L, seqNum++); //push table index
		GameObjectHandle* goh_lua = newudata<GameObjectHandle>(L, gameObjectHandleTypeName); //push copy of childs GOH
		*goh_lua = *goh;
		lua_settable(L, -3);
	}

	delete all;
	return 1;
}

static int lua_goloadgameworld(lua_State* L)
{
	I32 numArgs = lua_gettop(L);
	if (numArgs == 1)
	{
		const char * filename = luaL_checkstring(L, 1);
		kaleidoscope::GameObject::LoadGameWorld(filename);
		return 0;
	} 
	else if (numArgs == 2)
	{
		const char * filename = luaL_checkstring(L, 1);
		//bool overwrite = (lua_type(L, 2) == LUA_TBOOLEAN ? static_cast<bool>(lua_toboolean(L, 2)) : false);
		bool overwrite = (lua_type(L, 2) == LUA_TBOOLEAN ? (lua_toboolean(L, 1) != 0) : false);
		kaleidoscope::GameObject::LoadGameWorld(filename, overwrite);
		return 0;
	}
	return luaL_error(L, "argument error");
}

static int lua_gosavegameworld(lua_State* L)
{
	const char * filename = luaL_checkstring(L, 1);
	kaleidoscope::GameObject::SaveGameWorld(filename);
	return 0;
}

static int lua_gosavedynamicgameworld(lua_State* L)
{
	const char * filename = luaL_checkstring(L, 1);
	kaleidoscope::GameObject::SaveDynamicGameWorld(filename);
	return 0;
}

static int lua_goregistertag(lua_State* L)
{
	StringID tag = getstringid(L, 1);
	bool b = kaleidoscope::GameObject::RegisterTag(tag);
	lua_pushboolean(L, b);
	return 1;
}

static int lua_gounregistertag(lua_State* L)
{
	StringID tag = getstringid(L, 1);
	bool b = kaleidoscope::GameObject::UnregisterTag(tag);
	lua_pushboolean(L, b);
	return 1;
}

static int lua_gohvalid(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	lua_pushboolean(L, goh->valid());
	return 1;
}

static int lua_gohgetname(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	lua_pushnumber(L, goh->getName());
	return 1;
}

static int lua_gohenabled(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	lua_pushboolean(L, goh->enabled());
	return 1;
}

static int lua_gohenable(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	goh->enable();
	return 0;
}

static int lua_gohdisable(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	goh->disable();
	return 0;
}

static int lua_gohsettag(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	StringID tag = getstringid(L, 2);
	bool b = goh->setTag(tag);
	lua_pushboolean(L, b);
	return 1;
}

static int lua_gohcleartag(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	StringID tag = getstringid(L, 2);
	bool b = goh->clearTag(tag);
	lua_pushboolean(L, b);
	return 1;
}

static int lua_gohhastag(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	StringID tag = getstringid(L, 2);
	bool b = goh->hasTag(tag);
	lua_pushboolean(L, b);
	return 1;
}

static int lua_gohgetparent(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	GameObjectHandle* n = newGameObjectHandle(L);
	*n = goh->getParent();
	return 1;
}

static int lua_gohsetparent(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	GameObjectHandle* parent = getGameObjectHandle(L, 2);
	goh->setParent(*parent);
	return 0;
}

static int lua_gohgetchild(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	StringID name = getstringid(L, 2);
	GameObjectHandle* n = newGameObjectHandle(L);
	*n = goh->getChild(name);
	return 1;
}

static int lua_gohaddchild(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	GameObjectHandle* child = getGameObjectHandle(L, 2);
	goh->addChild(*child);
	return 0;
}

static int lua_gohremovechild(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);

	if (lua_type(L, 2) == LUA_TNUMBER)
	{
		StringID name = getstringid(L, 2);
		goh->removeChild(name);
		return 0;
	}
	else
	{
		GameObjectHandle* h = getGameObjectHandle(L, 2);
		goh->removeChild(*h);
		return 0;
	}

	return luaL_error(L, "argument error");
}

static int lua_gohaddcomponent(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	I32 componentType = luaL_checkint(L, 2);
	if (lua_gettop(L) == 3)
	{
		StringID filename = getstringid(L, 3);
		bool b = goh->addComponent(componentType, filename);
		lua_pushboolean(L, b);
		return 1;
	}
	else
	{
		bool b = goh->addComponent(componentType);
		lua_pushboolean(L, b);
		return 1;
	}
}

static int lua_gohremovecomponent(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	I32 componentType = luaL_checkint(L, 2);
	if (lua_gettop(L) == 3)
	{
		StringID filename = getstringid(L, 3);
		bool b = goh->removeComponent(componentType, filename);
		lua_pushboolean(L, b);
		return 1;
	}
	else
	{
		bool b = goh->removeComponent(componentType);
		lua_pushboolean(L, b);
		return 1;
	}
}

static int lua_gohtransform(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	TransformHandle* t = newtransformhandle(L);
	*t = goh->transform();
	return 1;
}

static int lua_gohscript(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	StringID filename = getstringid(L, 2);
	LuaScriptHandle* ls = newLUAScriptHandle(L);
	*ls = goh->script(filename);
	return 1;
}

static int lua_gohrenderable(lua_State* L)
{
	using kaleidoscope::Renderable;
	using kaleidoscope::RenderableHandle;

	GameObjectHandle* goh = getudata<GameObjectHandle>(L, gameObjectHandleTypeName, 1);
	RenderableHandle* rh = newudata<RenderableHandle>(L, RenderableHandle::LUA_TYPE_NAME);
	*rh = goh->renderable();
	return 1;
}

static int lua_goh_camera(lua_State* L)
{
	using kaleidoscope::Camera;
	using kaleidoscope::CameraHandle;

	GameObjectHandle* goh = getudata<GameObjectHandle>(L, gameObjectHandleTypeName, 1);
	CameraHandle* ch = newudata<CameraHandle>(L, CameraHandle::LUA_TYPE_NAME);
	*ch = goh->camera();
	return 1;
}

static int lua_goh_getChildren(lua_State* L)
{
	GameObjectHandle* goh = getudata<GameObjectHandle>(L, gameObjectHandleTypeName, 1);
	std::list<GameObjectHandle>* chldrn = goh->getChildren();

	lua_createtable(L, chldrn->size(), 0);

	I32 seqNum = 1;
	for (std::list<GameObjectHandle>::iterator child = chldrn->begin(); child != chldrn->end(); ++child)
	{
		lua_pushnumber(L, seqNum++); //push table index
		GameObjectHandle* child_lua = newudata<GameObjectHandle>(L, gameObjectHandleTypeName); //push copy of childs GOH
		*child_lua = *child;
		lua_settable(L, -3);
	}

	delete chldrn;
	return 1;
}

static int lua_gohprintstate(lua_State* L)
{
	GameObjectHandle* goh = getGameObjectHandle(L, 1);
	goh->printState();
	return 0;
}

static int lua_gohequal(lua_State* L)
{
	GameObjectHandle* lhs = getGameObjectHandle(L, 1);
	GameObjectHandle* rhs = getGameObjectHandle(L, 2);
	lua_pushboolean(L, (*lhs == *rhs));
	return 1;
}

static int lua_goaddnullhandle(lua_State* L)
{
	GameObjectHandle* null = newGameObjectHandle(L);
	*null = kaleidoscope::GameObjectHandle::null;
	lua_setfield(L, 2, "null");
	return 0;
}

static int lua_goaddname(lua_State* L)
{
	lua_pushnumber(L, kaleidoscope::GameObject::NAME);
	lua_setfield(L, 2, "NAME");
	return 0;
}

static int lua_enum(lua_State* L, const char * k, I32 v)
{
	lua_pushstring(L, k);
	lua_pushnumber(L, v);
	lua_settable(L, -3);
	return 0;
}

static const struct luaL_Reg go_sf[] = 
{
	{ "Create" , lua_gocreate },
	{ "Destroy", lua_godestroy },
	{ "SendEvent", lua_gosendevent },
	{ "BroadcastEvent", lua_gobroadcastevent },
	{ "FindByName", lua_gofindbyname },
	{ "FindByTag", lua_gofindbytag },
	{ "FindAllWithTag", lua_go_FindAllWithTag },
	{ "FindAll", lua_go_FindAll },
	{ "LoadGameWorld", lua_goloadgameworld },
	{ "SaveGameWorld", lua_gosavegameworld },
	{ "SaveDynamicGameWorld", lua_gosavedynamicgameworld },
	{ "RegisterTag", lua_goregistertag },
	{ "UnregisterTag", lua_gounregistertag },
	{ NULL , NULL }
};

static const struct luaL_Reg goh_mf[] = 
{
	{ "valid", lua_gohvalid },
	{ "getName", lua_gohgetname },
	{ "enabled", lua_gohenabled },
	{ "enable", lua_gohenable },
	{ "disable", lua_gohdisable },
	{ "setTag", lua_gohsettag },
	{ "clearTag", lua_gohcleartag },
	{ "hasTag", lua_gohhastag },
	{ "getParent", lua_gohgetparent },
	{ "setParent", lua_gohsetparent },
	{ "getChild", lua_gohgetchild },
	{ "getChildren", lua_goh_getChildren },
	{ "addChild", lua_gohaddchild },
	{ "removeChild", lua_gohremovechild },
	{ "addComponent", lua_gohaddcomponent },
	{ "removeComponent", lua_gohremovecomponent },
	{ "transform", lua_gohtransform },
	{ "script", lua_gohscript },
	{ "renderable", lua_gohrenderable },
	{ "camera", lua_goh_camera },
	{ "printState", lua_gohprintstate },
	{ "__eq", lua_gohequal },
	{ NULL, NULL }
};

int kaleidoscope::luaopen_GameObjectHandle(lua_State* L)
{

	luaL_newmetatable(L, gameObjectHandleTypeName);

	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	luaL_setfuncs(L, goh_mf, 0);

	luaL_newlib(L, go_sf);

	lua_goaddnullhandle(L);
	lua_goaddname(L);
	lua_setglobal(L, "GameObjectHandle");

	lua_pop(L, 1); // pop the metatable.
	return 0;
}