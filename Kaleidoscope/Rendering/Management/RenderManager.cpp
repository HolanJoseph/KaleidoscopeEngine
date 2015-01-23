#include <Rendering/Management/RenderManager.h>

#include <GL/glew.h>

#include <boost/optional.hpp>

#include <SDL_syswm.h>

#include <Rendering/Irrlicht/IrrlichtController.h>
#include <irrlicht.h>

#include <Utility/Parsing/parseMathsFromStrings.h>

#include <Components/Renderable/Renderable.h>
#include <Components/Light/Light.h>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;

namespace kaleidoscope
{

	RenderManager::RenderManager()
	{
	}


	RenderManager::~RenderManager()
	{
	}


	/*
	* bool kaleidoscope::RenderManager::startUp(boost::optional<const boost::property_tree::ptree&> info,
	*											boost::optional<const boost::property_tree::ptree&> cameraInfo,
	*											boost::optional<const boost::property_tree::ptree&> renderableInfo,
	*											boost::optional<const boost::property_tree::ptree&> lightInfo)
	*
	* In: optional<const ptree&> info : The properties for the renderer.
	* In: optional<const ptree&> cameraInfo : The properties for the camera component.
	* In: optional<const ptree&> renderableInfo : The properties for the renderable component.
	* In: optional<const ptree&> lightInfo : The properties for the light component.
	* Out: bool : true on success.
	*			  false on failure.
	*
	* Initializes everything the rendering system needs to function.
	*/
	bool RenderManager::startUp(boost::optional<const boost::property_tree::ptree&> info,
								boost::optional<const boost::property_tree::ptree&> cameraInfo,
								boost::optional<const boost::property_tree::ptree&> renderableInfo,
								boost::optional<const boost::property_tree::ptree&> lightInfo)
	{

		// Parse the rendering options from the properties.
		boost::optional<std::string> title = info->get_optional<std::string>("title");
		boost::optional<std::string> wndSz = info->get_optional<std::string>("size");
		boost::optional<std::string> wndDM = info->get_optional<std::string>("display mode");
		boost::optional<std::string> wndRM = info->get_optional<std::string>("swap mode");

		std::string wT;
		math::vec2 wS;
		GLWindow::DisplayType wDM;
		GLWindow::SwapMode wSM;

		(title ? wT = *title : wT = DEFAULTTITLE);
		bool b123;
		(wndSz ? wS = stringToVec2(wndSz->c_str(), b123) : DEFAULTSIZE );
		
		if (wndDM)
		{
			if (wndDM->compare("fullscreen") == 0)
			{
				wDM = GLWindow::DT_FULLSCREEN;
			}
			else if (wndDM->compare("windowed") == 0)
			{
				wDM = GLWindow::DT_WINDOWED;
			}
			else
			{
				wDM = DEFAULTDISPLAYTYPE;
			}
		}
		else
		{
			wDM = DEFAULTDISPLAYTYPE;
		}

		if (wndRM)
		{
			if (wndRM->compare("vsync") == 0)
			{
				wSM = GLWindow::SM_VSYNC;
			}
			else if (wndRM->compare("immediate") == 0)
			{
				wSM = GLWindow::SM_IMMEDIATE;
			}
			else if (wndRM->compare("tear") == 0)
			{
				wSM = GLWindow::SM_TEAR;
			}
			else
			{
				wSM = DEFAULTSWAPMODE;
			}
		}
		else
		{
			wSM = DEFAULTSWAPMODE;
		}

		// Create the window to render to.
		mWindow = GLWindow();
		if (!mWindow.init(wT.c_str(), wDM, wSM, wS))
		{
			return false;
		}
		mWindow.setClearColor(math::vec4(0.0f, 0.5f, 0.5f, 1.0f));

		mViewCamera = CameraHandle::null;
		mCullCamera = CameraHandle::null;

		SDL_SysWMinfo nfo;
		SDL_version compiledVersion;
		SDL_VERSION(&compiledVersion);
		nfo.version = compiledVersion;
		SDL_bool b =  SDL_GetWindowWMInfo( mWindow.getWindow(), &nfo);
		if (b != SDL_TRUE)
		{
			return false;
		}

		// Initialize Irrlicht Rendering System
		// Give Irrlicht access to the window.
		irr::SIrrlichtCreationParameters creationParams;
		creationParams.DriverType = irr::video::EDT_OPENGL;
		creationParams.WindowId = reinterpret_cast<void*>(nfo.info.win.window);
		creationParams.Vsync = true;

		if (mIrrController.createDevice(creationParams) == false)
		{
			gLogManager.log("Irrlicht setup Failed");
			return false;
		}

		// Start up the Camera component system.
		if (cameraInfo)
		{
			kaleidoscope::CameraHandle::StartUp(&mIrrController, *cameraInfo);
		}
		else
		{
			kaleidoscope::CameraHandle::StartUp(&mIrrController, boost::property_tree::ptree());
		}

		// Start up the Renderable component system.
		if (renderableInfo)
		{
			kaleidoscope::RenderableHandle::StartUp(&mIrrController, *renderableInfo);
		} 
		else
		{
			kaleidoscope::RenderableHandle::StartUp(&mIrrController, boost::property_tree::ptree());
		}

		// Start up the Light component system.
		if (lightInfo)
		{
			kaleidoscope::LightHandle::StartUp(&mIrrController, *lightInfo);
		}
		else
		{
			kaleidoscope::LightHandle::StartUp(&mIrrController, boost::property_tree::ptree());
		}

		return true;
	}


	/*
	* bool kaleidoscope::RenderManager::shutDown()
	*
	* shutdown the Render Manager system.
	*/
	bool RenderManager::shutDown()
	{

		kaleidoscope::CameraHandle::ShutDown();
		kaleidoscope::RenderableHandle::ShutDown();
		kaleidoscope::LightHandle::ShutDown();


		mIrrController.closeDevice();

		return true;
	}


	/*
	* void kaleidoscope::RenderManager::render()
	*
	* Draw all objects.
	*/
	void RenderManager::render()
	{
		CameraHandle::UpdateAll();
		RenderableHandle::UpdateAll();
		LightHandle::UpdateAll();

		mIrrController.drawAll();
	}

	/*
	* Camera functions currently are not integrated with Irrlicht, so they will do nothing.
	*/
	void RenderManager::setCamera(CameraHandle c)
	{
		mViewCamera = c;
		mCullCamera = c;
	}

	void RenderManager::setViewCamera(CameraHandle c)
	{
		mViewCamera = c;
	}

	CameraHandle RenderManager::getViewCamera() const { return mViewCamera; }

	void RenderManager::setCullCamera(CameraHandle c)
	{
		mCullCamera = c;
	}

	/*
	* kaleidoscope::math::vec2 kaleidoscope::RenderManager::getScreenDimensions() const
	* 
	* In: void :
	* Out: vec2 : screen dimensions as (w, h)
	*/
	math::vec2 RenderManager::getScreenDimensions() const
	{
		return mWindow.getRenderResolution();
	}

	CameraHandle RenderManager::getCullCamera() const { return mCullCamera; }

	/*
	* Test lighting.
	*/
	void RenderManager::enableLighting() { mIrrController.enableLighting(true); }
	void RenderManager::disableLighting() { mIrrController.enableLighting(true); }
	bool RenderManager::isLightingEnabled() const { return mIrrController.lightingEnabled(); }

	const char * RenderManager::DEFAULTTITLE = "Kaleidoscope Engine";
	const math::vec2 RenderManager::DEFAULTSIZE(640.0f, 480.0f);
	const GLWindow::DisplayType RenderManager::DEFAULTDISPLAYTYPE = GLWindow::DT_WINDOWED;
	const GLWindow::SwapMode RenderManager::DEFAULTSWAPMODE = GLWindow::SM_TEAR;
}