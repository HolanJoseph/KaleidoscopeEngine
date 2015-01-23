#include <Rendering/Base/GLWindow.h>

#include <GL/glew.h>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;

namespace kaleidoscope
{

	GLWindow::GLWindow() : mWindow(NULL), mContext(NULL), 
							mDT(DEFAULT_DISPLAYTYPE), mSM(DEFAULT_SWAPMODE),
							mRenderResolution(DEFAULT_RENDERRESOLUTION),
							mClearColor(DEFAULT_CLEARCOLOR)
	{
		mTitle = 0;
	}

	/*
	* bool kaleidoscope::GLWindow::init(const char * t, DisplayType dt, SwapMode sm, math::vec2 rr)
	* 
	* In: const char * t : A c string to be used as the window title.
	* In: DisplayType dt : The GLWindow::DisplayType to create the window.
	* In: SwapMode sm : The GLWindow::SwapMode to use to refresh the window.
	* In vec2 rr : How large to make the window specified as (w, h).
	* Out: bool : true on success.
	*			  false on failure.
	*/
	bool GLWindow::init(const char * t, DisplayType dt, SwapMode sm, math::vec2 rr)
	{
		mWindow = SDL_CreateWindow("", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, static_cast<int>(DEFAULT_RENDERRESOLUTION.x), static_cast<int>(DEFAULT_RENDERRESOLUTION.y), SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (mWindow == nullptr){
			gLogManager.log("SDL_CreateWindow Error: %s", SDL_GetError());
			return false;
		}

		mContext = SDL_GL_CreateContext(mWindow);
		if (mContext == NULL)
		{
			gLogManager.log("SDL_GL_CreateContext Error: %s", SDL_GetError());
			return false;
		}

		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			gLogManager.log("Error Initializing OpenGL extensions: %s", glewGetErrorString(err));
			SDL_Quit();
			return false;
		}
			
		setWindowTitle(t);
		setDisplayType(dt);
		setBufferSwapMode(sm);
		setRenderResolution(rr);
		return true;
	}

	GLWindow::~GLWindow()
	{
	}

	/*
	* SDL_Window* kaleidoscope::getWindow() const 
	* 
	* In: void :
	* Out: SDL_Window* : A pointer to this windows base SDL window.
	*/
	SDL_Window*   GLWindow::getWindow() const { return mWindow; }

	/*
	* SDL_GLContext kaleidoscope::getContext() const
	*
	* In: void :
	* Out: SDL_GLContext : the OpenGL Context for this window.
	*/
	SDL_GLContext GLWindow::getContext() const { return mContext; }


	/*
	* void kaleidoscope::GLWindow::setXXXXXX( ... )
	*
	* In: ... : The value to set the specified window property too.
	* Out: void :
	*/

	/*
	* XXX kaleidoscope::GLWindow::getXXXXXX() const
	*
	* In: void :
	* Out: XXX : The value of the specified property of the current window.
	*/
	void GLWindow::setWindowTitle(const char * t)
	{ 
		mTitle = internString(t);
		SDL_SetWindowTitle(mWindow, t);
	}
	StringID GLWindow::getWindowTitle() const { return mTitle; }

	void GLWindow::setDisplayType(DisplayType dt)
	{
		if (dt == mDT)
		{
			return;
		}

		mDT = dt;
		if (mDT == DT_WINDOWED)
		{
			SDL_SetWindowBordered(mWindow, SDL_TRUE);
			SDL_SetWindowFullscreen(mWindow, 0);
			SDL_SetWindowSize(mWindow, static_cast<int>(mRenderResolution.x), static_cast<int>(mRenderResolution.y));
			SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
		else if (mDT == DT_FULLSCREEN)
		{
			SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);
		}
	}
	GLWindow::DisplayType GLWindow::getDisplayType() const { return mDT; }

	void GLWindow::setRenderResolution(math::vec2 res)
	{
		if (mRenderResolution == res)
		{
			return;
		}

		mRenderResolution = res;
		SDL_SetWindowSize(mWindow, static_cast<I32>(mRenderResolution.x), static_cast<I32>(mRenderResolution.y));

		if (mDT == DT_WINDOWED)
		{
			SDL_SetWindowPosition(mWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
		}
		else if (mDT == DT_FULLSCREEN)
		{
			SDL_SetWindowFullscreen(mWindow, 0);
			SDL_SetWindowFullscreen(mWindow, SDL_WINDOW_FULLSCREEN);
		}
	}

	math::vec2 GLWindow::getRenderResolution() const { return mRenderResolution; }

	void GLWindow::setBufferSwapMode(SwapMode sm)
	{
		mSM = sm;
		I32 s;
		switch (sm)
		{
		case kaleidoscope::GLWindow::SM_IMMEDIATE:
			SDL_GL_SetSwapInterval(0);
			break;
		case kaleidoscope::GLWindow::SM_VSYNC:
			s = SDL_GL_SetSwapInterval(1);
			break;
		case kaleidoscope::GLWindow::SM_TEAR:
			s = SDL_GL_SetSwapInterval(-1);
			if (s == -1)
			{
				SDL_GL_SetSwapInterval(1);
			}
			break;
		default:
			break;
		}
	}

	GLWindow::SwapMode GLWindow::getBufferSwapMode() const { return mSM; }

	void GLWindow::setClearColor(math::vec4 c)
	{
		mClearColor = c;
		glClearColor(mClearColor.r, mClearColor.g, mClearColor.b, mClearColor.a);
	}

	math::vec4 GLWindow::getClearColor() const { return mClearColor; }

	void GLWindow::swapBuffer() { SDL_GL_SwapWindow(mWindow); }

	const math::vec2  GLWindow::DEFAULT_RENDERRESOLUTION(1280.0f, 720.0f);
	const math::vec4  GLWindow::DEFAULT_CLEARCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	const char *   GLWindow::DEFAULT_TITLE = "Kaleidoscope Engine";
	const GLWindow::DisplayType GLWindow::DEFAULT_DISPLAYTYPE = DT_WINDOWED;
	const GLWindow::SwapMode    GLWindow::DEFAULT_SWAPMODE = SM_VSYNC;

}