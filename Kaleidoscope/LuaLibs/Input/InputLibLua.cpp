#include <LuaLibs/Input/InputLibLua.h>

#include <Math/Math.h>

#include <Input/mousecodes.h>
#include <Input/scancodes.h>
#include <Input/HIDManager.h>
extern kaleidoscope::HIDManager gHIDManager;

#include <LuaLibs/Utility/stackDumpLua.h>

extern "C"
{
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

static int lua_setMousePosition(lua_State* L)
{
	if (lua_type(L,1) != LUA_TNUMBER)
	{
		gLogManager.log("setMousePosition: Argument 1 is not a number.");
		return 0;
	}
	if (lua_type(L,2) != LUA_TNUMBER)
	{
		gLogManager.log("setMousePosition: Argument 2 is not a number.");
		return 0;
	}

	int x = luaL_checkint(L, 1);
	int y = luaL_checkint(L, 2);
	gHIDManager.setMousePosition(kaleidoscope::math::ivec2(x, y));

	return 0;
}

static int lua_getMousePosition(lua_State* L)
{
	kaleidoscope::math::ivec2 pos = gHIDManager.getMousePosition();

	lua_pushnumber(L, pos.x);
	lua_pushnumber(L, pos.y);

	return 2;
}

static int lua_input_setMouseVisible(lua_State* L)
{
	gHIDManager.setMouseVisible();
	return 0;
}

static int lua_input_setMouseInvisible(lua_State* L)
{
	gHIDManager.setMouseInvisible();
	return 0;
}

static int lua_input_setMouseVisibility(lua_State* L)
{
	//bool isVis = static_cast<bool>(lua_toboolean(L, 1));
	bool isVis = (lua_toboolean(L, 1) != 0);
	gHIDManager.setMouseVisibility(isVis);
	return 0;
}

static int lua_input_isMouseVisible(lua_State* L)
{
	lua_pushboolean(L, gHIDManager.isMouseVisible());
	return 1;
}



static int lua_getKey(lua_State* L)
{
	if (lua_type(L, 1) != LUA_TNUMBER)
	{
		gLogManager.log("error getKey: expected numerical argument.");
	}
	int code = lua_tointeger(L, 1);


	bool state = false;
	if (code <= kaleidoscope::SCANCODE_APP1 && code >= kaleidoscope::SCANCODE_A)
	{
		state = gHIDManager.getKey(static_cast<kaleidoscope::KeyScancode>(code));
	} 
	else if (code <= kaleidoscope::SCANCODE_A && code >= kaleidoscope::MOUSECODE_LEFT)
	{
		state = gHIDManager.getKey(static_cast<kaleidoscope::MouseCode>(code));
	}


	if (state)
	{
		lua_pushboolean(L, true);
	} 
	else
	{
		lua_pushboolean(L, false);
	}

	return 1;
}


static int lua_getKeyDown(lua_State* L)
{
	if (lua_type(L, 1) != LUA_TNUMBER)
	{
		gLogManager.log("error getKey: expected numerical argument.");
	}
	int code = lua_tointeger(L, 1);


	bool state = false;
	if (code <= kaleidoscope::SCANCODE_APP1 && code >= kaleidoscope::SCANCODE_A)
	{
		state = gHIDManager.getKeyDown(static_cast<kaleidoscope::KeyScancode>(code));
	}
	else if (code <= kaleidoscope::SCANCODE_A && code >= kaleidoscope::MOUSECODE_LEFT)
	{
		state = gHIDManager.getKeyDown(static_cast<kaleidoscope::MouseCode>(code));
	}


	if (state)
	{
		lua_pushboolean(L, true);
	}
	else
	{
		lua_pushboolean(L, false);
	}

	return 1;
}


static int lua_getKeyUp(lua_State* L)
{
	if (lua_type(L, 1) != LUA_TNUMBER)
	{
		gLogManager.log("error getKey: expected numerical argument.");
	}
	int code = lua_tointeger(L, 1);


	bool state = false;
	if (code <= kaleidoscope::SCANCODE_APP1 && code >= kaleidoscope::SCANCODE_A)
	{
		state = gHIDManager.getKeyUp(static_cast<kaleidoscope::KeyScancode>(code));
	}
	else if (code <= kaleidoscope::SCANCODE_A && code >= kaleidoscope::MOUSECODE_LEFT)
	{
		state = gHIDManager.getKeyUp(static_cast<kaleidoscope::MouseCode>(code));
	}


	if (state)
	{
		lua_pushboolean(L, true);
	}
	else
	{
		lua_pushboolean(L, false);
	}

	return 1;
}


static int lua_enum(lua_State* L, const char * k, I32 v)
{
	lua_pushstring(L, k);
	lua_pushnumber(L, v);
	lua_settable(L, -3);
	return 0;
}


static int lua_addmousecodes(lua_State* L)
{
	lua_newtable(L);
	lua_enum(L, "LEFT"  , 1);
	lua_enum(L, "MIDDLE", 2);
	lua_enum(L, "RIGHT" , 3);
	lua_setfield(L, 1, "Mousecode");
	return 0;
}


static int lua_addscancodes(lua_State* L)
{
	lua_newtable(L);

	lua_enum(L, "A", kaleidoscope::SCANCODE_A);
	lua_enum(L, "B", kaleidoscope::SCANCODE_B);
	lua_enum(L, "C", kaleidoscope::SCANCODE_C);
	lua_enum(L, "D", kaleidoscope::SCANCODE_D);
	lua_enum(L, "E", kaleidoscope::SCANCODE_E);
	lua_enum(L, "F", kaleidoscope::SCANCODE_F);
	lua_enum(L, "G", kaleidoscope::SCANCODE_G);
	lua_enum(L, "H", kaleidoscope::SCANCODE_H);
	lua_enum(L, "I", kaleidoscope::SCANCODE_I);
	lua_enum(L, "J", kaleidoscope::SCANCODE_J);
	lua_enum(L, "K", kaleidoscope::SCANCODE_K);
	lua_enum(L, "L", kaleidoscope::SCANCODE_L);
	lua_enum(L, "M", kaleidoscope::SCANCODE_M);
	lua_enum(L, "N", kaleidoscope::SCANCODE_N);
	lua_enum(L, "O", kaleidoscope::SCANCODE_O);
	lua_enum(L, "P", kaleidoscope::SCANCODE_P);
	lua_enum(L, "Q", kaleidoscope::SCANCODE_Q);
	lua_enum(L, "R", kaleidoscope::SCANCODE_R);
	lua_enum(L, "S", kaleidoscope::SCANCODE_S);
	lua_enum(L, "T", kaleidoscope::SCANCODE_T);
	lua_enum(L, "U", kaleidoscope::SCANCODE_U);
	lua_enum(L, "V", kaleidoscope::SCANCODE_V);
	lua_enum(L, "W", kaleidoscope::SCANCODE_W);
	lua_enum(L, "X", kaleidoscope::SCANCODE_X);
	lua_enum(L, "Y", kaleidoscope::SCANCODE_Y);
	lua_enum(L, "Z", kaleidoscope::SCANCODE_Z);


	lua_enum(L, "1", kaleidoscope::SCANCODE_1);
	lua_enum(L, "2", kaleidoscope::SCANCODE_2);
	lua_enum(L, "3", kaleidoscope::SCANCODE_3);
	lua_enum(L, "4", kaleidoscope::SCANCODE_4);
	lua_enum(L, "5", kaleidoscope::SCANCODE_5);
	lua_enum(L, "6", kaleidoscope::SCANCODE_6);
	lua_enum(L, "7", kaleidoscope::SCANCODE_7);
	lua_enum(L, "8", kaleidoscope::SCANCODE_8);
	lua_enum(L, "9", kaleidoscope::SCANCODE_9);
	lua_enum(L, "0", kaleidoscope::SCANCODE_0);


	lua_enum(L, "RETURN", kaleidoscope::SCANCODE_RETURN);
	lua_enum(L, "ESCAPE", kaleidoscope::SCANCODE_ESCAPE);
	lua_enum(L, "BACKSPACE", kaleidoscope::SCANCODE_BACKSPACE);
	lua_enum(L, "TAB", kaleidoscope::SCANCODE_TAB);
	lua_enum(L, "SPACE", kaleidoscope::SCANCODE_SPACE);
	lua_enum(L, "MINUS", kaleidoscope::SCANCODE_MINUS);
	lua_enum(L, "EQUALS", kaleidoscope::SCANCODE_EQUALS);
	lua_enum(L, "LEFTBRACKET", kaleidoscope::SCANCODE_LEFTBRACKET);
	lua_enum(L, "RIGHTBRACKET", kaleidoscope::SCANCODE_RIGHTBRACKET);
	lua_enum(L, "BACKSLASH", kaleidoscope::SCANCODE_BACKSLASH);
	lua_enum(L, "NONUSHASH", kaleidoscope::SCANCODE_NONUSHASH);
	lua_enum(L, "SEMICOLON", kaleidoscope::SCANCODE_SEMICOLON);
	lua_enum(L, "APOSTROPHE", kaleidoscope::SCANCODE_APOSTROPHE);
	lua_enum(L, "GRAVE", kaleidoscope::SCANCODE_GRAVE);
	lua_enum(L, "COMMA", kaleidoscope::SCANCODE_COMMA);
	lua_enum(L, "PERIOD", kaleidoscope::SCANCODE_PERIOD);
	lua_enum(L, "SLASH", kaleidoscope::SCANCODE_SLASH);
	lua_enum(L, "CAPSLOCK", kaleidoscope::SCANCODE_CAPSLOCK);


	lua_enum(L, "F1", kaleidoscope::SCANCODE_F1);
	lua_enum(L, "F2", kaleidoscope::SCANCODE_F2);
	lua_enum(L, "F3", kaleidoscope::SCANCODE_F3);
	lua_enum(L, "F4", kaleidoscope::SCANCODE_F4);
	lua_enum(L, "F5", kaleidoscope::SCANCODE_F5);
	lua_enum(L, "F6", kaleidoscope::SCANCODE_F7);
	lua_enum(L, "F8", kaleidoscope::SCANCODE_F8);
	lua_enum(L, "F9", kaleidoscope::SCANCODE_F9);
	lua_enum(L, "F10", kaleidoscope::SCANCODE_F10);
	lua_enum(L, "F11", kaleidoscope::SCANCODE_F11);
	lua_enum(L, "F12", kaleidoscope::SCANCODE_F12);


	lua_enum(L, "PRINTSCREEN", kaleidoscope::SCANCODE_PRINTSCREEN);
	lua_enum(L, "SCROLLLOCK", kaleidoscope::SCANCODE_SCROLLLOCK);
	lua_enum(L, "PAUSE", kaleidoscope::SCANCODE_PAUSE);
	lua_enum(L, "INSERT", kaleidoscope::SCANCODE_INSERT);
	lua_enum(L, "HOME", kaleidoscope::SCANCODE_HOME);
	lua_enum(L, "PAGEUP", kaleidoscope::SCANCODE_PAGEUP);
	lua_enum(L, "DELETE", kaleidoscope::SCANCODE_DELETE);
	lua_enum(L, "END", kaleidoscope::SCANCODE_END);
	lua_enum(L, "PAGEDOWN", kaleidoscope::SCANCODE_PAGEDOWN);
	lua_enum(L, "RIGHT", kaleidoscope::SCANCODE_RIGHT);
	lua_enum(L, "LEFT", kaleidoscope::SCANCODE_LEFT);
	lua_enum(L, "DOWN", kaleidoscope::SCANCODE_DOWN);
	lua_enum(L, "UP", kaleidoscope::SCANCODE_UP);
	lua_enum(L, "NUMLOCKCLEAR", kaleidoscope::SCANCODE_NUMLOCKCLEAR);
	lua_enum(L, "KP_DIVIDE", kaleidoscope::SCANCODE_KP_DIVIDE);
	lua_enum(L, "KP_MULTIPLY", kaleidoscope::SCANCODE_KP_MULTIPLY);
	lua_enum(L, "KP_MINUS", kaleidoscope::SCANCODE_KP_MINUS);
	lua_enum(L, "KP_PLUS", kaleidoscope::SCANCODE_KP_PLUS);
	lua_enum(L, "KP_ENTER", kaleidoscope::SCANCODE_KP_ENTER);
	lua_enum(L, "KP_1", kaleidoscope::SCANCODE_KP_1);
	lua_enum(L, "KP_2", kaleidoscope::SCANCODE_KP_2);
	lua_enum(L, "KP_3", kaleidoscope::SCANCODE_KP_3);
	lua_enum(L, "KP_4", kaleidoscope::SCANCODE_KP_4);
	lua_enum(L, "KP_5", kaleidoscope::SCANCODE_KP_5);
	lua_enum(L, "KP_6", kaleidoscope::SCANCODE_KP_6);
	lua_enum(L, "KP_7", kaleidoscope::SCANCODE_KP_7);
	lua_enum(L, "KP_8", kaleidoscope::SCANCODE_KP_8);
	lua_enum(L, "KP_9", kaleidoscope::SCANCODE_KP_9);
	lua_enum(L, "KP_0", kaleidoscope::SCANCODE_KP_0);
	lua_enum(L, "KP_PERIOD", kaleidoscope::SCANCODE_KP_PERIOD);
	lua_enum(L, "NONUSBACKSLASH", kaleidoscope::SCANCODE_NONUSBACKSLASH);
	lua_enum(L, "APPLICATION", kaleidoscope::SCANCODE_APPLICATION);
	lua_enum(L, "POWER", kaleidoscope::SCANCODE_POWER);
	lua_enum(L, "KP_EQUALS", kaleidoscope::SCANCODE_KP_EQUALS);
	lua_enum(L, "F13", kaleidoscope::SCANCODE_F13);
	lua_enum(L, "F14", kaleidoscope::SCANCODE_F14);
	lua_enum(L, "F15", kaleidoscope::SCANCODE_F15);
	lua_enum(L, "F16", kaleidoscope::SCANCODE_F16);
	lua_enum(L, "F17", kaleidoscope::SCANCODE_F17);
	lua_enum(L, "F18", kaleidoscope::SCANCODE_F18);
	lua_enum(L, "F19", kaleidoscope::SCANCODE_F19);
	lua_enum(L, "F20", kaleidoscope::SCANCODE_F20);
	lua_enum(L, "F21", kaleidoscope::SCANCODE_F21);
	lua_enum(L, "F22", kaleidoscope::SCANCODE_F22);
	lua_enum(L, "F23", kaleidoscope::SCANCODE_F23);
	lua_enum(L, "F24", kaleidoscope::SCANCODE_F24);
	lua_enum(L, "EXECUTE", kaleidoscope::SCANCODE_EXECUTE);
	lua_enum(L, "HELP", kaleidoscope::SCANCODE_HELP);
	lua_enum(L, "MENU", kaleidoscope::SCANCODE_MENU);
	lua_enum(L, "SELECT", kaleidoscope::SCANCODE_SELECT);
	lua_enum(L, "STOP", kaleidoscope::SCANCODE_STOP);
	lua_enum(L, "AGAIN", kaleidoscope::SCANCODE_AGAIN);
	lua_enum(L, "UNDO", kaleidoscope::SCANCODE_UNDO);
	lua_enum(L, "CUT", kaleidoscope::SCANCODE_CUT);
	lua_enum(L, "COPY", kaleidoscope::SCANCODE_COPY);
	lua_enum(L, "PASTE", kaleidoscope::SCANCODE_PASTE);
	lua_enum(L, "FIND", kaleidoscope::SCANCODE_FIND);
	lua_enum(L, "MUTE", kaleidoscope::SCANCODE_MUTE);
	lua_enum(L, "VOLUMEUP", kaleidoscope::SCANCODE_VOLUMEUP);
	lua_enum(L, "VOLUMEDOWN", kaleidoscope::SCANCODE_VOLUMEDOWN);
	lua_enum(L, "KP_COMMA", kaleidoscope::SCANCODE_KP_COMMA);
	lua_enum(L, "KP_EQUALSAS400", kaleidoscope::SCANCODE_KP_EQUALSAS400);


	lua_enum(L, "INTERNATIONAL1", kaleidoscope::SCANCODE_INTERNATIONAL1);
	lua_enum(L, "INTERNATIONAL2", kaleidoscope::SCANCODE_INTERNATIONAL2);
	lua_enum(L, "INTERNATIONAL3", kaleidoscope::SCANCODE_INTERNATIONAL3);
	lua_enum(L, "INTERNATIONAL4", kaleidoscope::SCANCODE_INTERNATIONAL4);
	lua_enum(L, "INTERNATIONAL5", kaleidoscope::SCANCODE_INTERNATIONAL5);
	lua_enum(L, "INTERNATIONAL6", kaleidoscope::SCANCODE_INTERNATIONAL6);
	lua_enum(L, "INTERNATIONAL7", kaleidoscope::SCANCODE_INTERNATIONAL7);
	lua_enum(L, "INTERNATIONAL8", kaleidoscope::SCANCODE_INTERNATIONAL8);
	lua_enum(L, "INTERNATIONAL9", kaleidoscope::SCANCODE_INTERNATIONAL9);
	lua_enum(L, "LANG1", kaleidoscope::SCANCODE_LANG1);
	lua_enum(L, "LANG2", kaleidoscope::SCANCODE_LANG2);
	lua_enum(L, "LANG3", kaleidoscope::SCANCODE_LANG3);
	lua_enum(L, "LANG4", kaleidoscope::SCANCODE_LANG4);
	lua_enum(L, "LANG5", kaleidoscope::SCANCODE_LANG5);
	lua_enum(L, "LANG6", kaleidoscope::SCANCODE_LANG6);
	lua_enum(L, "LANG7", kaleidoscope::SCANCODE_LANG7);
	lua_enum(L, "LANG8", kaleidoscope::SCANCODE_LANG8);
	lua_enum(L, "LANG9", kaleidoscope::SCANCODE_LANG9);


	lua_enum(L, "ALTERASE", kaleidoscope::SCANCODE_ALTERASE);
	lua_enum(L, "SYSREQ", kaleidoscope::SCANCODE_SYSREQ);
	lua_enum(L, "CANCEL", kaleidoscope::SCANCODE_CANCEL);
	lua_enum(L, "CLEAR", kaleidoscope::SCANCODE_CLEAR);
	lua_enum(L, "PRIOR", kaleidoscope::SCANCODE_PRIOR);
	lua_enum(L, "RETURN2", kaleidoscope::SCANCODE_RETURN2);
	lua_enum(L, "SEPARATOR", kaleidoscope::SCANCODE_SEPARATOR);
	lua_enum(L, "OUT", kaleidoscope::SCANCODE_OUT);
	lua_enum(L, "OPER", kaleidoscope::SCANCODE_OPER);
	lua_enum(L, "CLEARAGAIN", kaleidoscope::SCANCODE_CLEARAGAIN);
	lua_enum(L, "CRSEL", kaleidoscope::SCANCODE_CRSEL);
	lua_enum(L, "EXSEL", kaleidoscope::SCANCODE_EXSEL);


	lua_enum(L, "KP_00", kaleidoscope::SCANCODE_KP_00);
	lua_enum(L, "KP_000", kaleidoscope::SCANCODE_KP_000);
	lua_enum(L, "THOUSANDSSEPARATOR", kaleidoscope::SCANCODE_THOUSANDSSEPARATOR);
	lua_enum(L, "DECIMALSEPARATOR", kaleidoscope::SCANCODE_DECIMALSEPARATOR);
	lua_enum(L, "CURRENCYUNIT", kaleidoscope::SCANCODE_CURRENCYUNIT);
	lua_enum(L, "CURRENCYSUBUNIT", kaleidoscope::SCANCODE_CURRENCYSUBUNIT);
	lua_enum(L, "KP_LEFTPAREN", kaleidoscope::SCANCODE_KP_LEFTPAREN);
	lua_enum(L, "KP_RIGHTPAREN", kaleidoscope::SCANCODE_KP_RIGHTPAREN);
	lua_enum(L, "KP_LEFTBRACE", kaleidoscope::SCANCODE_KP_LEFTBRACE);
	lua_enum(L, "KP_RIGHTBRACE", kaleidoscope::SCANCODE_KP_RIGHTBRACE);
	lua_enum(L, "KP_TAB", kaleidoscope::SCANCODE_KP_TAB);
	lua_enum(L, "KP_BACKSPACE", kaleidoscope::SCANCODE_KP_BACKSPACE);
	lua_enum(L, "KP_A", kaleidoscope::SCANCODE_KP_A);
	lua_enum(L, "KP_B", kaleidoscope::SCANCODE_KP_B);
	lua_enum(L, "KP_C", kaleidoscope::SCANCODE_KP_C);
	lua_enum(L, "KP_D", kaleidoscope::SCANCODE_KP_D);
	lua_enum(L, "KP_E", kaleidoscope::SCANCODE_KP_E);
	lua_enum(L, "KP_F", kaleidoscope::SCANCODE_KP_F);
	lua_enum(L, "KP_XOR", kaleidoscope::SCANCODE_KP_XOR);
	lua_enum(L, "KP_POWER", kaleidoscope::SCANCODE_KP_POWER);
	lua_enum(L, "KP_PERCENT", kaleidoscope::SCANCODE_KP_PERCENT);
	lua_enum(L, "KP_LESS", kaleidoscope::SCANCODE_KP_LESS);
	lua_enum(L, "KP_GREATER", kaleidoscope::SCANCODE_KP_GREATER);
	lua_enum(L, "KP_AMPERSAND", kaleidoscope::SCANCODE_KP_AMPERSAND);
	lua_enum(L, "KP_DBLAMPERSAND", kaleidoscope::SCANCODE_KP_DBLAMPERSAND);
	lua_enum(L, "KP_VERTICALBAR", kaleidoscope::SCANCODE_KP_VERTICALBAR);
	lua_enum(L, "KP_DBLVERTICALBAR", kaleidoscope::SCANCODE_KP_DBLVERTICALBAR);
	lua_enum(L, "KP_COLON", kaleidoscope::SCANCODE_KP_COLON);
	lua_enum(L, "KP_HASH", kaleidoscope::SCANCODE_KP_HASH);
	lua_enum(L, "KP_SPACE", kaleidoscope::SCANCODE_KP_SPACE);
	lua_enum(L, "KP_AT", kaleidoscope::SCANCODE_KP_AT);
	lua_enum(L, "KP_EXCLAM", kaleidoscope::SCANCODE_KP_EXCLAM);
	lua_enum(L, "KP_MEMSTORE", kaleidoscope::SCANCODE_KP_MEMSTORE);
	lua_enum(L, "KP_MEMRECALL", kaleidoscope::SCANCODE_KP_MEMRECALL);
	lua_enum(L, "KP_MEMCLEAR", kaleidoscope::SCANCODE_KP_MEMCLEAR);
	lua_enum(L, "KP_MEMADD", kaleidoscope::SCANCODE_KP_MEMADD);
	lua_enum(L, "KP_MEMSUBTRACT", kaleidoscope::SCANCODE_KP_MEMSUBTRACT);
	lua_enum(L, "KP_MEMMULTIPLY", kaleidoscope::SCANCODE_KP_MEMMULTIPLY);
	lua_enum(L, "KP_MEMDIVIDE", kaleidoscope::SCANCODE_KP_MEMDIVIDE);
	lua_enum(L, "KP_PLUSMINUS", kaleidoscope::SCANCODE_KP_PLUSMINUS);
	lua_enum(L, "KP_CLEAR", kaleidoscope::SCANCODE_KP_CLEAR);
	lua_enum(L, "KP_CLEARENTRY", kaleidoscope::SCANCODE_KP_CLEARENTRY);
	lua_enum(L, "KP_BINARY", kaleidoscope::SCANCODE_KP_BINARY);
	lua_enum(L, "KP_OCTAL", kaleidoscope::SCANCODE_KP_OCTAL);
	lua_enum(L, "KP_DECIMAL", kaleidoscope::SCANCODE_KP_DECIMAL);
	lua_enum(L, "KP_HEXADECIMAL", kaleidoscope::SCANCODE_KP_HEXADECIMAL);


	lua_enum(L, "LCTRL", kaleidoscope::SCANCODE_LCTRL);
	lua_enum(L, "LSHIFT", kaleidoscope::SCANCODE_LSHIFT);
	lua_enum(L, "LALT", kaleidoscope::SCANCODE_LALT);
	lua_enum(L, "LGUI", kaleidoscope::SCANCODE_LGUI);
	lua_enum(L, "RCTRL", kaleidoscope::SCANCODE_RCTRL);
	lua_enum(L, "RSHIFT", kaleidoscope::SCANCODE_RSHIFT);
	lua_enum(L, "RALT", kaleidoscope::SCANCODE_RALT);
	lua_enum(L, "RGUI", kaleidoscope::SCANCODE_RGUI);
	lua_enum(L, "MODE", kaleidoscope::SCANCODE_MODE);


	lua_enum(L, "AUDIONEXT", kaleidoscope::SCANCODE_AUDIONEXT);
	lua_enum(L, "AUDIOPREV", kaleidoscope::SCANCODE_AUDIOPREV);
	lua_enum(L, "AUDIOSTOP", kaleidoscope::SCANCODE_AUDIOSTOP);
	lua_enum(L, "AUDIOPLAY", kaleidoscope::SCANCODE_AUDIOPLAY);
	lua_enum(L, "AUDIOMUTE", kaleidoscope::SCANCODE_AUDIOMUTE);
	lua_enum(L, "MEDIASELECT", kaleidoscope::SCANCODE_MEDIASELECT);
	lua_enum(L, "WWW", kaleidoscope::SCANCODE_WWW);
	lua_enum(L, "MAIL", kaleidoscope::SCANCODE_MAIL);
	lua_enum(L, "CALCULATOR", kaleidoscope::SCANCODE_CALCULATOR);
	lua_enum(L, "COMPUTER", kaleidoscope::SCANCODE_COMPUTER);
	lua_enum(L, "AC_SEARCH", kaleidoscope::SCANCODE_AC_SEARCH);
	lua_enum(L, "AC_HOME", kaleidoscope::SCANCODE_AC_HOME);
	lua_enum(L, "AC_BACK", kaleidoscope::SCANCODE_AC_BACK);
	lua_enum(L, "AC_FORWARD", kaleidoscope::SCANCODE_AC_FORWARD);
	lua_enum(L, "AC_STOP", kaleidoscope::SCANCODE_AC_STOP);
	lua_enum(L, "AC_REFRESH", kaleidoscope::SCANCODE_AC_REFRESH);
	lua_enum(L, "AC_BOOKMARKS", kaleidoscope::SCANCODE_AC_BOOKMARKS);


	lua_enum(L, "BRIGHTNESSDOWN", kaleidoscope::SCANCODE_BRIGHTNESSDOWN);
	lua_enum(L, "BRIGHTNESSUP", kaleidoscope::SCANCODE_BRIGHTNESSUP);
	lua_enum(L, "DISPLAYSWITCH", kaleidoscope::SCANCODE_DISPLAYSWITCH);
	lua_enum(L, "KBDILLUMTOGGLE", kaleidoscope::SCANCODE_KBDILLUMTOGGLE);
	lua_enum(L, "KBDILLUMDOWN", kaleidoscope::SCANCODE_KBDILLUMDOWN);
	lua_enum(L, "KBDILLUMUP", kaleidoscope::SCANCODE_KBDILLUMUP);
	lua_enum(L, "EJECT", kaleidoscope::SCANCODE_EJECT);
	lua_enum(L, "SLEEP", kaleidoscope::SCANCODE_SLEEP);


	lua_enum(L, "APP1", kaleidoscope::SCANCODE_APP1);
	lua_enum(L, "APP2", kaleidoscope::SCANCODE_APP2);


	lua_setfield(L, 1, "Scancode");
	return 0;
}


static const struct luaL_Reg kInput[] =
{
	{ "setMousePosition", lua_setMousePosition },
	{ "getMousePosition", lua_getMousePosition },
	{ "setMouseVisible", lua_input_setMouseVisible },
	{ "setMouseInvisible", lua_input_setMouseInvisible },
	{ "setMouseVisibility", lua_input_setMouseVisibility },
	{ "isMouseVisible", lua_input_isMouseVisible },
	{ "getKey", lua_getKey },
	{ "getKeyDown", lua_getKeyDown },
	{ "getKeyUp", lua_getKeyUp },
	{ NULL, NULL }
};

int kaleidoscope::luaopen_kInput(lua_State* L)
{
	luaL_newlib(L, kInput);
	//lua_setglobal(L, "kInput");
	//lua_getglobal(L, "kInput");
	lua_addscancodes(L);
	lua_addmousecodes(L);
	lua_setglobal(L, "kInput");
	//lua_pop(L, 1);
	return 0;
}