#pragma once

#include <SDL_events.h>
#include <SDL_scancode.h>

#include <Input/scancodes.h>
#include <Input/mousecodes.h>

#include <bitset>
#include <queue>

#include <glm/glm.hpp>

namespace kaleidoscope
{
	class HIDManager
	{
	public:

		// The SDLLogManager is a pseudo singleton class with an empty constructor and destructor.
		//  To initialize an instance of SDLLogManager you must call one of the init functions. Before 
		//	the destructor is called the shutdown() function should be called to release all of the classes
		//	data.
		HIDManager(){};
		~HIDManager(){};
		bool init();
		bool shutdown();

		// These 3 functions, along with setMousePosition make up the interface for the main engine loop. 
		//	As the main loop receives keyboard events it should pass them to the HID Manager to be processed.
		void queueEvent(const SDL_KeyboardEvent& e);
		void queueEvent(const SDL_MouseButtonEvent& e);
		void processEvents(); // must be called after all events have been added or internal state will not be updated.

		// The following 5 functions make up the gameplay interface. Allows for querying of mouse position and key states.
		//void setMousePosition(const glm::vec2& pos);
		void setMousePosition(glm::ivec2 pos);
		glm::ivec2 getMousePosition() const;

		void setMouseVisible();
		void setMouseInvisible();
		void setMouseVisibility(bool isVisible);
		bool isMouseVisible() const;

		bool getKey(KeyScancode code) const; // Returns true if the requested key is pressed on the current frame.
		bool getKey(MouseCode code) const;
		bool getKeyDown(KeyScancode code) const; // Returns true if the key was pressed this frame, will return false on subsequent frames.
		bool getKeyDown(MouseCode code) const;
		bool getKeyUp(KeyScancode code) const; // Returns true the frame after the requested key has been release.
		bool getKeyUp(MouseCode code) const;

	private:

		// This class acts as an adapter between SDL and the HID Manager. 
		class keyEvent
		{
		public:

			enum KeyState
			{
				KEY_RELEASED,
				KEY_PRESSED
			};

			keyEvent(const SDL_KeyboardEvent &e);

			KeyState mKeyState;
			KeyScancode mScancode;
		};

		class mouseEvent
		{
		public:

			enum KeyState
			{
				KEY_RELEASED,
				KEY_PRESSED
			};

			mouseEvent(const SDL_MouseButtonEvent &e);

			KeyState mKeyState;
			MouseCode mMousecode;
		};

		std::bitset<NUM_SCANCODES> mKeyStates;
		std::bitset<NUM_SCANCODES> mKeysUpdatedThisFrame;

		glm::ivec2 mMousePosition;
		std::bitset<NUM_MOUSECODES> mMouseButtonStates;
		std::bitset<NUM_MOUSECODES> mMouseButtonsUpdatedThisFrame;

		std::queue<keyEvent> mKeyEventQueue;
		std::queue<mouseEvent> mMouseEventQueue;
	};
}