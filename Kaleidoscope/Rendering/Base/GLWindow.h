#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

#include <SDL.h>

namespace kaleidoscope
{
	class GLWindow
	{
	public:
		enum DisplayType
		{
			DT_FULLSCREEN,
			DT_WINDOWED
		};

		enum SwapMode
		{
			SM_IMMEDIATE,
			SM_VSYNC,
			SM_TEAR
		};

		GLWindow();
		bool init(const char * t, DisplayType dt, SwapMode sm, math::vec2 rr);
		~GLWindow();

		SDL_Window*   getWindow() const;
		SDL_GLContext getContext() const;

		void setWindowTitle(const char * t);
		StringID getWindowTitle() const;

		void setDisplayType(DisplayType dt);
		DisplayType getDisplayType() const;

		void setRenderResolution(math::vec2 res);
		math::vec2 getRenderResolution() const;

		void setBufferSwapMode(SwapMode sm);
		SwapMode getBufferSwapMode() const;

		void setClearColor(math::vec4 c);
		math::vec4 getClearColor() const;

		void swapBuffer();

	private:
		SDL_Window* mWindow;
		SDL_GLContext mContext;

		DisplayType mDT;
		SwapMode  mSM;

		math::vec2 mRenderResolution;
		math::vec4 mClearColor;

		StringID mTitle;

		static const math::vec2  DEFAULT_RENDERRESOLUTION;
		static const math::vec4  DEFAULT_CLEARCOLOR;
		static const char *      DEFAULT_TITLE;
		static const DisplayType DEFAULT_DISPLAYTYPE;
		static const SwapMode    DEFAULT_SWAPMODE;

	};
}