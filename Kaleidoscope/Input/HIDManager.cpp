#include <Input/HIDManager.h>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager lm;

namespace kaleidoscope
{

	HIDManager::keyEvent::keyEvent(const SDL_KeyboardEvent &e)
	{
		switch (e.state)
		{
		case SDL_PRESSED:
			mKeyState = KEY_PRESSED;
			break;
		case SDL_RELEASED:
			mKeyState = KEY_RELEASED;
			break;
		default:
			mKeyState = KEY_RELEASED;
			break;
		}

		switch (e.keysym.scancode)
		{
		case SDL_SCANCODE_UNKNOWN:
			mScancode = SCANCODE_UNKNOWN;
			break;
		case SDL_SCANCODE_A:
			mScancode = SCANCODE_A;
			break;
		case SDL_SCANCODE_B:
			mScancode = SCANCODE_B;
			break;
		case SDL_SCANCODE_C:
			mScancode = SCANCODE_C;
			break;
		case SDL_SCANCODE_D:
			mScancode = SCANCODE_D;
			break;
		case SDL_SCANCODE_E:
			mScancode = SCANCODE_E;
			break;
		case SDL_SCANCODE_F:
			mScancode = SCANCODE_F;
			break;
		case SDL_SCANCODE_G:
			mScancode = SCANCODE_G;
			break;
		case SDL_SCANCODE_H:
			mScancode = SCANCODE_H;
			break;
		case SDL_SCANCODE_I:
			mScancode = SCANCODE_I;
			break;
		case SDL_SCANCODE_J:
			mScancode = SCANCODE_J;
			break;
		case SDL_SCANCODE_K:
			mScancode = SCANCODE_K;
			break;
		case SDL_SCANCODE_L:
			mScancode = SCANCODE_L;
			break;
		case SDL_SCANCODE_M:
			mScancode = SCANCODE_M;
			break;
		case SDL_SCANCODE_N:
			mScancode = SCANCODE_N;
			break;
		case SDL_SCANCODE_O:
			mScancode = SCANCODE_O;
			break;
		case SDL_SCANCODE_P:
			mScancode = SCANCODE_P;
			break;
		case SDL_SCANCODE_Q:
			mScancode = SCANCODE_Q;
			break;
		case SDL_SCANCODE_R:
			mScancode = SCANCODE_R;
			break;
		case SDL_SCANCODE_S:
			mScancode = SCANCODE_S;
			break;
		case SDL_SCANCODE_T:
			mScancode = SCANCODE_T;
			break;
		case SDL_SCANCODE_U:
			mScancode = SCANCODE_U;
			break;
		case SDL_SCANCODE_V:
			mScancode = SCANCODE_V;
			break;
		case SDL_SCANCODE_W:
			mScancode = SCANCODE_W;
			break;
		case SDL_SCANCODE_X:
			mScancode = SCANCODE_X;
			break;
		case SDL_SCANCODE_Y:
			mScancode = SCANCODE_Y;
			break;
		case SDL_SCANCODE_Z:
			mScancode = SCANCODE_Z;
			break;
		case SDL_SCANCODE_1:
			mScancode = SCANCODE_1;
			break;
		case SDL_SCANCODE_2:
			mScancode = SCANCODE_2;
			break;
		case SDL_SCANCODE_3:
			mScancode = SCANCODE_3;
			break;
		case SDL_SCANCODE_4:
			mScancode = SCANCODE_4;
			break;
		case SDL_SCANCODE_5:
			mScancode = SCANCODE_5;
			break;
		case SDL_SCANCODE_6:
			mScancode = SCANCODE_6;
			break;
		case SDL_SCANCODE_7:
			mScancode = SCANCODE_7;
			break;
		case SDL_SCANCODE_8:
			mScancode = SCANCODE_8;
			break;
		case SDL_SCANCODE_9:
			mScancode = SCANCODE_9;
			break;
		case SDL_SCANCODE_0:
			mScancode = SCANCODE_0;
			break;
		case SDL_SCANCODE_RETURN:
			mScancode = SCANCODE_RETURN;
			break;
		case SDL_SCANCODE_ESCAPE:
			mScancode = SCANCODE_ESCAPE;
			break;
		case SDL_SCANCODE_BACKSPACE:
			mScancode = SCANCODE_BACKSPACE;
			break;
		case SDL_SCANCODE_TAB:
			mScancode = SCANCODE_TAB;
			break;
		case SDL_SCANCODE_SPACE:
			mScancode = SCANCODE_SPACE;
			break;
		case SDL_SCANCODE_MINUS:
			mScancode = SCANCODE_MINUS;
			break;
		case SDL_SCANCODE_EQUALS:
			mScancode = SCANCODE_EQUALS;
			break;
		case SDL_SCANCODE_LEFTBRACKET:
			mScancode = SCANCODE_LEFTBRACKET;
			break;
		case SDL_SCANCODE_RIGHTBRACKET:
			mScancode = SCANCODE_RIGHTBRACKET;
			break;
		case SDL_SCANCODE_BACKSLASH:
			mScancode = SCANCODE_BACKSLASH;
			break;
		case SDL_SCANCODE_NONUSHASH:
			mScancode = SCANCODE_NONUSHASH;
			break;
		case SDL_SCANCODE_SEMICOLON:
			mScancode = SCANCODE_SEMICOLON;
			break;
		case SDL_SCANCODE_APOSTROPHE:
			mScancode = SCANCODE_APOSTROPHE;
			break;
		case SDL_SCANCODE_GRAVE:
			mScancode = SCANCODE_GRAVE;
			break;
		case SDL_SCANCODE_COMMA:
			mScancode = SCANCODE_COMMA;
			break;
		case SDL_SCANCODE_PERIOD:
			mScancode = SCANCODE_PERIOD;
			break;
		case SDL_SCANCODE_SLASH:
			mScancode = SCANCODE_SLASH;
			break;
		case SDL_SCANCODE_CAPSLOCK:
			mScancode = SCANCODE_CAPSLOCK;
			break;
		case SDL_SCANCODE_F1:
			mScancode = SCANCODE_F1;
			break;
		case SDL_SCANCODE_F2:
			mScancode = SCANCODE_F2;
			break;
		case SDL_SCANCODE_F3:
			mScancode = SCANCODE_F3;
			break;
		case SDL_SCANCODE_F4:
			mScancode = SCANCODE_F4;
			break;
		case SDL_SCANCODE_F5:
			mScancode = SCANCODE_F5;
			break;
		case SDL_SCANCODE_F6:
			mScancode = SCANCODE_F6;
			break;
		case SDL_SCANCODE_F7:
			mScancode = SCANCODE_F7;
			break;
		case SDL_SCANCODE_F8:
			mScancode = SCANCODE_F8;
			break;
		case SDL_SCANCODE_F9:
			mScancode = SCANCODE_F9;
			break;
		case SDL_SCANCODE_F10:
			mScancode = SCANCODE_F10;
			break;
		case SDL_SCANCODE_F11:
			mScancode = SCANCODE_F11;
			break;
		case SDL_SCANCODE_F12:
			mScancode = SCANCODE_F12;
			break;
		case SDL_SCANCODE_PRINTSCREEN:
			mScancode = SCANCODE_PRINTSCREEN;
			break;
		case SDL_SCANCODE_SCROLLLOCK:
			mScancode = SCANCODE_SCROLLLOCK;
			break;
		case SDL_SCANCODE_PAUSE:
			mScancode = SCANCODE_PAUSE;
			break;
		case SDL_SCANCODE_INSERT:
			mScancode = SCANCODE_INSERT;
			break;
		case SDL_SCANCODE_HOME:
			mScancode = SCANCODE_HOME;
			break;
		case SDL_SCANCODE_PAGEUP:
			mScancode = SCANCODE_PAGEUP;
			break;
		case SDL_SCANCODE_DELETE:
			mScancode = SCANCODE_DELETE;
			break;
		case SDL_SCANCODE_END:
			mScancode = SCANCODE_END;
			break;
		case SDL_SCANCODE_PAGEDOWN:
			mScancode = SCANCODE_PAGEDOWN;
			break;
		case SDL_SCANCODE_RIGHT:
			mScancode = SCANCODE_RIGHT;
			break;
		case SDL_SCANCODE_LEFT:
			mScancode = SCANCODE_LEFT;
			break;
		case SDL_SCANCODE_DOWN:
			mScancode = SCANCODE_DOWN;
			break;
		case SDL_SCANCODE_UP:
			mScancode = SCANCODE_UP;
			break;
		case SDL_SCANCODE_NUMLOCKCLEAR:
			mScancode = SCANCODE_NUMLOCKCLEAR;
			break;
		case SDL_SCANCODE_KP_DIVIDE:
			mScancode = SCANCODE_KP_DIVIDE;
			break;
		case SDL_SCANCODE_KP_MULTIPLY:
			mScancode = SCANCODE_KP_MULTIPLY;
			break;
		case SDL_SCANCODE_KP_MINUS:
			mScancode = SCANCODE_KP_MINUS;
			break;
		case SDL_SCANCODE_KP_PLUS:
			mScancode = SCANCODE_KP_PLUS;
			break;
		case SDL_SCANCODE_KP_ENTER:
			mScancode = SCANCODE_KP_ENTER;
			break;
		case SDL_SCANCODE_KP_1:
			mScancode = SCANCODE_KP_1;
			break;
		case SDL_SCANCODE_KP_2:
			mScancode = SCANCODE_KP_2;
			break;
		case SDL_SCANCODE_KP_3:
			mScancode = SCANCODE_KP_3;
			break;
		case SDL_SCANCODE_KP_4:
			mScancode = SCANCODE_KP_4;
			break;
		case SDL_SCANCODE_KP_5:
			mScancode = SCANCODE_KP_5;
			break;
		case SDL_SCANCODE_KP_6:
			mScancode = SCANCODE_KP_6;
			break;
		case SDL_SCANCODE_KP_7:
			mScancode = SCANCODE_KP_7;
			break;
		case SDL_SCANCODE_KP_8:
			mScancode = SCANCODE_KP_8;
			break;
		case SDL_SCANCODE_KP_9:
			mScancode = SCANCODE_KP_9;
			break;
		case SDL_SCANCODE_KP_0:
			mScancode = SCANCODE_KP_0;
			break;
		case SDL_SCANCODE_KP_PERIOD:
			mScancode = SCANCODE_KP_PERIOD;
			break;
		case SDL_SCANCODE_NONUSBACKSLASH:
			mScancode = SCANCODE_NONUSBACKSLASH;
			break;
		case SDL_SCANCODE_APPLICATION:
			mScancode = SCANCODE_APPLICATION;
			break;
		case SDL_SCANCODE_POWER:
			mScancode = SCANCODE_POWER;
			break;
		case SDL_SCANCODE_KP_EQUALS:
			mScancode = SCANCODE_KP_EQUALS;
			break;
		case SDL_SCANCODE_F13:
			mScancode = SCANCODE_F13;
			break;
		case SDL_SCANCODE_F14:
			mScancode = SCANCODE_F14;
			break;
		case SDL_SCANCODE_F15:
			mScancode = SCANCODE_F15;
			break;
		case SDL_SCANCODE_F16:
			mScancode = SCANCODE_F16;
			break;
		case SDL_SCANCODE_F17:
			mScancode = SCANCODE_F17;
			break;
		case SDL_SCANCODE_F18:
			mScancode = SCANCODE_F18;
			break;
		case SDL_SCANCODE_F19:
			mScancode = SCANCODE_F19;
			break;
		case SDL_SCANCODE_F20:
			mScancode = SCANCODE_F20;
			break;
		case SDL_SCANCODE_F21:
			mScancode = SCANCODE_F21;
			break;
		case SDL_SCANCODE_F22:
			mScancode = SCANCODE_F22;
			break;
		case SDL_SCANCODE_F23:
			mScancode = SCANCODE_F23;
			break;
		case SDL_SCANCODE_F24:
			mScancode = SCANCODE_F24;
			break;
		case SDL_SCANCODE_EXECUTE:
			mScancode = SCANCODE_EXECUTE;
			break;
		case SDL_SCANCODE_HELP:
			mScancode = SCANCODE_HELP;
			break;
		case SDL_SCANCODE_MENU:
			mScancode = SCANCODE_MENU;
			break;
		case SDL_SCANCODE_SELECT:
			mScancode = SCANCODE_SELECT;
			break;
		case SDL_SCANCODE_STOP:
			mScancode = SCANCODE_STOP;
			break;
		case SDL_SCANCODE_AGAIN:
			mScancode = SCANCODE_AGAIN;
			break;
		case SDL_SCANCODE_UNDO:
			mScancode = SCANCODE_UNDO;
			break;
		case SDL_SCANCODE_CUT:
			mScancode = SCANCODE_CUT;
			break;
		case SDL_SCANCODE_COPY:
			mScancode = SCANCODE_COPY;
			break;
		case SDL_SCANCODE_PASTE:
			mScancode = SCANCODE_PASTE;
			break;
		case SDL_SCANCODE_FIND:
			mScancode = SCANCODE_FIND;
			break;
		case SDL_SCANCODE_MUTE:
			mScancode = SCANCODE_MUTE;
			break;
		case SDL_SCANCODE_VOLUMEUP:
			mScancode = SCANCODE_VOLUMEUP;
			break;
		case SDL_SCANCODE_VOLUMEDOWN:
			mScancode = SCANCODE_VOLUMEDOWN;
			break;
		case SDL_SCANCODE_KP_COMMA:
			mScancode = SCANCODE_KP_COMMA;
			break;
		case SDL_SCANCODE_KP_EQUALSAS400:
			mScancode = SCANCODE_KP_EQUALSAS400;
			break;
		case SDL_SCANCODE_INTERNATIONAL1:
			mScancode = SCANCODE_INTERNATIONAL1;
			break;
		case SDL_SCANCODE_INTERNATIONAL2:
			mScancode = SCANCODE_INTERNATIONAL2;
			break;
		case SDL_SCANCODE_INTERNATIONAL3:
			mScancode = SCANCODE_INTERNATIONAL3;
			break;
		case SDL_SCANCODE_INTERNATIONAL4:
			mScancode = SCANCODE_INTERNATIONAL4;
			break;
		case SDL_SCANCODE_INTERNATIONAL5:
			mScancode = SCANCODE_INTERNATIONAL5;
			break;
		case SDL_SCANCODE_INTERNATIONAL6:
			mScancode = SCANCODE_INTERNATIONAL6;
			break;
		case SDL_SCANCODE_INTERNATIONAL7:
			mScancode = SCANCODE_INTERNATIONAL7;
			break;
		case SDL_SCANCODE_INTERNATIONAL8:
			mScancode = SCANCODE_INTERNATIONAL8;
			break;
		case SDL_SCANCODE_INTERNATIONAL9:
			mScancode = SCANCODE_INTERNATIONAL9;
			break;
		case SDL_SCANCODE_LANG1:
			mScancode = SCANCODE_LANG1;
			break;
		case SDL_SCANCODE_LANG2:
			mScancode = SCANCODE_LANG2;
			break;
		case SDL_SCANCODE_LANG3:
			mScancode = SCANCODE_LANG3;
			break;
		case SDL_SCANCODE_LANG4:
			mScancode = SCANCODE_LANG4;
			break;
		case SDL_SCANCODE_LANG5:
			mScancode = SCANCODE_LANG5;
			break;
		case SDL_SCANCODE_LANG6:
			mScancode = SCANCODE_LANG6;
			break;
		case SDL_SCANCODE_LANG7:
			mScancode = SCANCODE_LANG7;
			break;
		case SDL_SCANCODE_LANG8:
			mScancode = SCANCODE_LANG8;
			break;
		case SDL_SCANCODE_LANG9:
			mScancode = SCANCODE_LANG9;
			break;
		case SDL_SCANCODE_ALTERASE:
			mScancode = SCANCODE_ALTERASE;
			break;
		case SDL_SCANCODE_SYSREQ:
			mScancode = SCANCODE_SYSREQ;
			break;
		case SDL_SCANCODE_CANCEL:
			mScancode = SCANCODE_CANCEL;
			break;
		case SDL_SCANCODE_CLEAR:
			mScancode = SCANCODE_CLEAR;
			break;
		case SDL_SCANCODE_PRIOR:
			mScancode = SCANCODE_PRIOR;
			break;
		case SDL_SCANCODE_RETURN2:
			mScancode = SCANCODE_RETURN2;
			break;
		case SDL_SCANCODE_SEPARATOR:
			mScancode = SCANCODE_SEPARATOR;
			break;
		case SDL_SCANCODE_OUT:
			mScancode = SCANCODE_OUT;
			break;
		case SDL_SCANCODE_OPER:
			mScancode = SCANCODE_OPER;
			break;
		case SDL_SCANCODE_CLEARAGAIN:
			mScancode = SCANCODE_CLEARAGAIN;
			break;
		case SDL_SCANCODE_CRSEL:
			mScancode = SCANCODE_CRSEL;
			break;
		case SDL_SCANCODE_EXSEL:
			mScancode = SCANCODE_EXSEL;
			break;
		case SDL_SCANCODE_KP_00:
			mScancode = SCANCODE_KP_00;
			break;
		case SDL_SCANCODE_KP_000:
			mScancode = SCANCODE_KP_000;
			break;
		case SDL_SCANCODE_THOUSANDSSEPARATOR:
			mScancode = SCANCODE_THOUSANDSSEPARATOR;
			break;
		case SDL_SCANCODE_DECIMALSEPARATOR:
			mScancode = SCANCODE_DECIMALSEPARATOR;
			break;
		case SDL_SCANCODE_CURRENCYUNIT:
			mScancode = SCANCODE_CURRENCYUNIT;
			break;
		case SDL_SCANCODE_CURRENCYSUBUNIT:
			mScancode = SCANCODE_CURRENCYSUBUNIT;
			break;
		case SDL_SCANCODE_KP_LEFTPAREN:
			mScancode = SCANCODE_KP_LEFTPAREN;
			break;
		case SDL_SCANCODE_KP_RIGHTPAREN:
			mScancode = SCANCODE_KP_RIGHTPAREN;
			break;
		case SDL_SCANCODE_KP_LEFTBRACE:
			mScancode = SCANCODE_KP_LEFTBRACE;
			break;
		case SDL_SCANCODE_KP_RIGHTBRACE:
			mScancode = SCANCODE_KP_RIGHTBRACE;
			break;
		case SDL_SCANCODE_KP_TAB:
			mScancode = SCANCODE_KP_TAB;
			break;
		case SDL_SCANCODE_KP_BACKSPACE:
			mScancode = SCANCODE_KP_BACKSPACE;
			break;
		case SDL_SCANCODE_KP_A:
			mScancode = SCANCODE_KP_A;
			break;
		case SDL_SCANCODE_KP_B:
			mScancode = SCANCODE_KP_B;
			break;
		case SDL_SCANCODE_KP_C:
			mScancode = SCANCODE_KP_C;
			break;
		case SDL_SCANCODE_KP_D:
			mScancode = SCANCODE_KP_D;
			break;
		case SDL_SCANCODE_KP_E:
			mScancode = SCANCODE_KP_E;
			break;
		case SDL_SCANCODE_KP_F:
			mScancode = SCANCODE_KP_F;
			break;
		case SDL_SCANCODE_KP_XOR:
			mScancode = SCANCODE_KP_XOR;
			break;
		case SDL_SCANCODE_KP_POWER:
			mScancode = SCANCODE_KP_POWER;
			break;
		case SDL_SCANCODE_KP_PERCENT:
			mScancode = SCANCODE_KP_PERCENT;
			break;
		case SDL_SCANCODE_KP_LESS:
			mScancode = SCANCODE_KP_LESS;
			break;
		case SDL_SCANCODE_KP_GREATER:
			mScancode = SCANCODE_KP_GREATER;
			break;
		case SDL_SCANCODE_KP_AMPERSAND:
			mScancode = SCANCODE_KP_AMPERSAND;
			break;
		case SDL_SCANCODE_KP_DBLAMPERSAND:
			mScancode = SCANCODE_KP_DBLAMPERSAND;
			break;
		case SDL_SCANCODE_KP_VERTICALBAR:
			mScancode = SCANCODE_KP_VERTICALBAR;
			break;
		case SDL_SCANCODE_KP_DBLVERTICALBAR:
			mScancode = SCANCODE_KP_DBLVERTICALBAR;
			break;
		case SDL_SCANCODE_KP_COLON:
			mScancode = SCANCODE_KP_COLON;
			break;
		case SDL_SCANCODE_KP_HASH:
			mScancode = SCANCODE_KP_HASH;
			break;
		case SDL_SCANCODE_KP_SPACE:
			mScancode = SCANCODE_KP_SPACE;
			break;
		case SDL_SCANCODE_KP_AT:
			mScancode = SCANCODE_KP_AT;
			break;
		case SDL_SCANCODE_KP_EXCLAM:
			mScancode = SCANCODE_KP_EXCLAM;
			break;
		case SDL_SCANCODE_KP_MEMSTORE:
			mScancode = SCANCODE_KP_MEMSTORE;
			break;
		case SDL_SCANCODE_KP_MEMRECALL:
			mScancode = SCANCODE_KP_MEMRECALL;
			break;
		case SDL_SCANCODE_KP_MEMCLEAR:
			mScancode = SCANCODE_KP_MEMCLEAR;
			break;
		case SDL_SCANCODE_KP_MEMADD:
			mScancode = SCANCODE_KP_MEMADD;
			break;
		case SDL_SCANCODE_KP_MEMSUBTRACT:
			mScancode = SCANCODE_KP_MEMSUBTRACT;
			break;
		case SDL_SCANCODE_KP_MEMMULTIPLY:
			mScancode = SCANCODE_KP_MEMMULTIPLY;
			break;
		case SDL_SCANCODE_KP_MEMDIVIDE:
			mScancode = SCANCODE_KP_MEMDIVIDE;
			break;
		case SDL_SCANCODE_KP_PLUSMINUS:
			mScancode = SCANCODE_KP_PLUSMINUS;
			break;
		case SDL_SCANCODE_KP_CLEAR:
			mScancode = SCANCODE_KP_CLEAR;
			break;
		case SDL_SCANCODE_KP_CLEARENTRY:
			mScancode = SCANCODE_KP_CLEARENTRY;
			break;
		case SDL_SCANCODE_KP_BINARY:
			mScancode = SCANCODE_KP_BINARY;
			break;
		case SDL_SCANCODE_KP_OCTAL:
			mScancode = SCANCODE_KP_OCTAL;
			break;
		case SDL_SCANCODE_KP_DECIMAL:
			mScancode = SCANCODE_KP_DECIMAL;
			break;
		case SDL_SCANCODE_KP_HEXADECIMAL:
			mScancode = SCANCODE_KP_HEXADECIMAL;
			break;
		case SDL_SCANCODE_LCTRL:
			mScancode = SCANCODE_LCTRL;
			break;
		case SDL_SCANCODE_LSHIFT:
			mScancode = SCANCODE_LSHIFT;
			break;
		case SDL_SCANCODE_LALT:
			mScancode = SCANCODE_LALT;
			break;
		case SDL_SCANCODE_LGUI:
			mScancode = SCANCODE_LGUI;
			break;
		case SDL_SCANCODE_RCTRL:
			mScancode = SCANCODE_RCTRL;
			break;
		case SDL_SCANCODE_RSHIFT:
			mScancode = SCANCODE_RSHIFT;
			break;
		case SDL_SCANCODE_RALT:
			mScancode = SCANCODE_RALT;
			break;
		case SDL_SCANCODE_RGUI:
			mScancode = SCANCODE_RGUI;
			break;
		case SDL_SCANCODE_MODE:
			mScancode = SCANCODE_MODE;
			break;
		case SDL_SCANCODE_AUDIONEXT:
			mScancode = SCANCODE_AUDIONEXT;
			break;
		case SDL_SCANCODE_AUDIOPREV:
			mScancode = SCANCODE_AUDIOPREV;
			break;
		case SDL_SCANCODE_AUDIOSTOP:
			mScancode = SCANCODE_AUDIOSTOP;
			break;
		case SDL_SCANCODE_AUDIOPLAY:
			mScancode = SCANCODE_AUDIOPLAY;
			break;
		case SDL_SCANCODE_AUDIOMUTE:
			mScancode = SCANCODE_AUDIOMUTE;
			break;
		case SDL_SCANCODE_MEDIASELECT:
			mScancode = SCANCODE_MEDIASELECT;
			break;
		case SDL_SCANCODE_WWW:
			mScancode = SCANCODE_WWW;
			break;
		case SDL_SCANCODE_MAIL:
			mScancode = SCANCODE_MAIL;
			break;
		case SDL_SCANCODE_CALCULATOR:
			mScancode = SCANCODE_CALCULATOR;
			break;
		case SDL_SCANCODE_COMPUTER:
			mScancode = SCANCODE_COMPUTER;
			break;
		case SDL_SCANCODE_AC_SEARCH:
			mScancode = SCANCODE_AC_SEARCH;
			break;
		case SDL_SCANCODE_AC_HOME:
			mScancode = SCANCODE_AC_HOME;
			break;
		case SDL_SCANCODE_AC_BACK:
			mScancode = SCANCODE_AC_BACK;
			break;
		case SDL_SCANCODE_AC_FORWARD:
			mScancode = SCANCODE_AC_FORWARD;
			break;
		case SDL_SCANCODE_AC_STOP:
			mScancode = SCANCODE_AC_STOP;
			break;
		case SDL_SCANCODE_AC_REFRESH:
			mScancode = SCANCODE_AC_REFRESH;
			break;
		case SDL_SCANCODE_AC_BOOKMARKS:
			mScancode = SCANCODE_AC_BOOKMARKS;
			break;
		case SDL_SCANCODE_BRIGHTNESSDOWN:
			mScancode = SCANCODE_BRIGHTNESSDOWN;
			break;
		case SDL_SCANCODE_BRIGHTNESSUP:
			mScancode = SCANCODE_BRIGHTNESSUP;
			break;
		case SDL_SCANCODE_DISPLAYSWITCH:
			mScancode = SCANCODE_DISPLAYSWITCH;
			break;
		case SDL_SCANCODE_KBDILLUMTOGGLE:
			mScancode = SCANCODE_KBDILLUMTOGGLE;
			break;
		case SDL_SCANCODE_KBDILLUMDOWN:
			mScancode = SCANCODE_KBDILLUMDOWN;
			break;
		case SDL_SCANCODE_KBDILLUMUP:
			mScancode = SCANCODE_KBDILLUMUP;
			break;
		case SDL_SCANCODE_EJECT:
			mScancode = SCANCODE_EJECT;
			break;
		case SDL_SCANCODE_SLEEP:
			mScancode = SCANCODE_SLEEP;
			break;
		case SDL_SCANCODE_APP1:
			mScancode = SCANCODE_APP1;
			break;
		case SDL_SCANCODE_APP2:
			mScancode = SCANCODE_APP2;
			break;
		default:
			break;
		}
	}

	HIDManager::mouseEvent::mouseEvent(const SDL_MouseButtonEvent &e)
	{
		switch (e.state)
		{
		case SDL_PRESSED:
			mKeyState = KEY_PRESSED;
			break;
		case SDL_RELEASED:
			mKeyState = KEY_RELEASED;
			break;
		default:
			mKeyState = KEY_RELEASED;
			break;
		}

		switch (e.button)
		{
		case SDL_BUTTON_LEFT:
			mMousecode = MOUSECODE_LEFT;
			break;
		case SDL_BUTTON_MIDDLE:
			mMousecode = MOUSECODE_MIDDLE;
			break;
		case SDL_BUTTON_RIGHT:
			mMousecode = MOUSECODE_RIGHT;
			break;
		default:
			break;
		}

	}

	/*
	* bool kaleidoscope::HIDManager::init()
	* 
	* In: void :
	* Out: bool : true on success.
	*			  false on failure.
	*
	* Startup the input recording and querying system.
	*/
	bool HIDManager::init()
	{
		mKeyStates = std::bitset<NUM_SCANCODES>();
		mKeysUpdatedThisFrame = std::bitset<NUM_SCANCODES>();

		mMousePosition = glm::ivec2(0,0);
		mMouseButtonStates = std::bitset<NUM_MOUSECODES>();
		mMouseButtonsUpdatedThisFrame = std::bitset<NUM_MOUSECODES>();

		mKeyEventQueue = std::queue<keyEvent>();
		mMouseEventQueue = std::queue<mouseEvent>();

		return true;
	}


	/*
	* bool kaleidoscope::HIDManager::shutdown()
	*
	* In: void :
	* Out: bool : true on success.
	*			  false on failure.
	*
	* shutdown the input recording and querying system.
	*/
	bool HIDManager::shutdown()
	{
		return true;
	}


	/*
	* void kaleidoscope::HIDManager::queueEvent(const SDL_KeyboardEvent& e)
	*
	* In: const SDL_KeyboardEvent& e : A reference to an SDL key event.
	* Out: void :
	* 
	* converts the SDL event to a kaleid event and adds it to the queue of events to be processed 
	*	in HIDManager::processEvents().
	*/
	void HIDManager::queueEvent(const SDL_KeyboardEvent& e)
	{
		mKeyEventQueue.push(keyEvent(e));
	}


	/*
	* void kaleidoscope::HIDManager::queueEvent(const SDL_MouseButtonEvent& e)
	*
	* In: const SDL_MouseButtonEvent& e : A reference to an SDL mouse event.
	* Out: void :
	*
	* Converts the SDL event to a kaleid event and adds it to the queue of events to be processed
	*	in HIDManager::processEvents().
	*/
	void HIDManager::queueEvent(const SDL_MouseButtonEvent& e)
	{
		mMouseEventQueue.push(mouseEvent(e));
	}


	/*
	* void kaleidoscope::HIDManager::processEvents()
	*
	* In: void :
	* Out: void: 
	*
	* Process all of the queued mouse and key events to update the state of the keyboard.
	* Should only be called once per frame.
	*/
	void HIDManager::processEvents()
	{
		// Process Key Events.
		mKeysUpdatedThisFrame.reset();
		while (!mKeyEventQueue.empty())
		{
			keyEvent& e = mKeyEventQueue.front();

			if ( (e.mKeyState == keyEvent::KEY_PRESSED) && (!mKeyStates.test(e.mScancode)) )
			{
				mKeyStates.set(e.mScancode, true);
				mKeysUpdatedThisFrame.set(e.mScancode, true);
			}

			if (e.mKeyState == keyEvent::KEY_RELEASED)
			{
				mKeyStates.set(e.mScancode, false);
				mKeysUpdatedThisFrame.set(e.mScancode, true);
			}

			mKeyEventQueue.pop();
		}

		// Process Mouse Events.
		mMouseButtonsUpdatedThisFrame.reset();
		while (!mMouseEventQueue.empty())
		{
			mouseEvent& e = mMouseEventQueue.front();

			if ((e.mKeyState == keyEvent::KEY_PRESSED) && (!mMouseButtonStates.test(e.mMousecode)))
			{
				mMouseButtonStates.set(e.mMousecode, true);
				mMouseButtonsUpdatedThisFrame.set(e.mMousecode, true);
			}

			if (e.mKeyState == keyEvent::KEY_RELEASED)
			{
				mMouseButtonStates.set(e.mMousecode, false);
				mMouseButtonsUpdatedThisFrame.set(e.mMousecode, true);
			}

			mMouseEventQueue.pop();
		}

	}


	/*
	* void kaleidoscope::HIDManager::setMousePosition(glm::ivec2 pos)
	*
	* In: ivec2 pos: the new position of the cursor.
	* Out: void :
	*
	* The position is specified in pixels with with the upper left hand corner at (0, 0)
	*	+x going to the right
	*	+y going down 
	*
	* The position is not updated until the NEXT frame. So setting the position and querying it on the same frame will result 
	*	in likely unintended behavior.
	*/
	void HIDManager::setMousePosition(glm::ivec2 pos)
	{
		SDL_WarpMouseInWindow(NULL, pos.x, pos.y);
	}

	/*
	* glm::ivec2 kaleidoscope::HIDManager::getMousePosition() const
	*
	* In: void :
	* Out: ivec2 : The position of the mouse AS QUERIED THIS FRAME.
	*
	* The position is specified in pixels with with the upper left hand corner at (0, 0)
	*	+x going to the right
	*	+y going down
	*
	* The mouse position is updated the frame after the a setMousePoisiton see HIDManager::setMousePosition 
	*	for more.
	*/
	glm::ivec2 HIDManager::getMousePosition() const
	{
		int x;
		int y;
		SDL_GetMouseState(&x, &y);
		return glm::ivec2(x, y);
	}

	/*
	* void kaleidoscope::HIDManager::setMouseVisible()
	* 
	* These next 3 functions control the visibility of the mouse cursor.
	*/
	void HIDManager::setMouseVisible()
	{
		SDL_ShowCursor(1);
	}

	void HIDManager::setMouseInvisible()
	{
		SDL_ShowCursor(0);
	}

	void HIDManager::setMouseVisibility(bool setVisible)
	{
		if (setVisible)
		{
			setMouseVisible();
		}
		else
		{
			setMouseInvisible();
		}
	}

	bool HIDManager::isMouseVisible() const
	{
		int visibility = SDL_ShowCursor(-1);

		if (visibility == 1)
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	/*
	* bool kaleidscope::HIDManager::getKey(kaleidscope::KeyScancode code) const
	*
	* In: Scancode code : The Scancode to query.
	* Out: bool : true if the key is down this frame.
	*			  false if the key is not down this frame.
	*/
	bool HIDManager::getKey(KeyScancode code) const
	{
		return mKeyStates.test(code);
	}

	bool HIDManager::getKey(MouseCode code) const
	{
		return mMouseButtonStates.test(code);
	}


	/*
	* bool kaleidscope::HIDManager::getKeyDown(kaleidscope::KeyScancode code) const
	*
	* In: Scancode code : The Scancode to query.
	* Out: bool : true if the key is down this frame and was not down on the previous frame.
	*			  false if the key is not down this frame. Or the key is down and was also down on the previous frame.
	*/
	bool HIDManager::getKeyDown(KeyScancode code) const
	{
		if (mKeyStates.test(code) && mKeysUpdatedThisFrame.test(code))
		{
			return true;
		} 
		else
		{
			return false;
		}
	}

	bool HIDManager::getKeyDown(MouseCode code) const
	{
		if (mMouseButtonStates.test(code) && mMouseButtonsUpdatedThisFrame.test(code))
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	/*
	* bool kaleidscope::HIDManager::getKeyUp(kaleidscope::KeyScancode code) const
	*
	* In: Scancode code : The Scancode to query.
	* Out: bool : true if the key is not down this frame and was down on the previous frame.
	*			  false if the key is down this frame. Or the key is not down and was also not down on the previous frame.
	*/
	bool HIDManager::getKeyUp(KeyScancode code) const
	{
		if (!mKeyStates.test(code) && mKeysUpdatedThisFrame.test(code))
		{
			return true;
		} 
		else
		{
			return false;
		}
	}

	bool HIDManager::getKeyUp(MouseCode code) const
	{
		if (!mMouseButtonStates.test(code) && mMouseButtonsUpdatedThisFrame.test(code))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
}