#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

#include <SDL.h>

#include <boost/property_tree/ptree.hpp>

#include <irrlicht.h>

#include <Rendering/Base/GLWindow.h>

#include <Components/Camera/CameraHandle.h>
#include <Components/Renderable/RenderableHandle.h>
#include <Components/Light/LightHandle.h>

namespace kaleidoscope
{
	class RenderManager
	{
	public:
		RenderManager();
		~RenderManager();

		bool startUp(boost::optional<const boost::property_tree::ptree&> info,
			 		 boost::optional<const boost::property_tree::ptree&> cameraInfo,
					 boost::optional<const boost::property_tree::ptree&> renderableInfo,
					 boost::optional<const boost::property_tree::ptree&> lightInfo);
		bool shutDown();

		void render();

		void setCamera(CameraHandle c);

		void setViewCamera(CameraHandle c);
		CameraHandle getViewCamera() const;

		void setCullCamera(CameraHandle c);
		CameraHandle getCullCamera() const;

		math::vec2 getScreenDimensions() const;

		void enableLighting();
		void disableLighting();
		bool isLightingEnabled() const;

	private:
		GLWindow mWindow;
		IrrlichtController mIrrController;

		irr::IrrlichtDevice* mDevice;
		irr::scene::ISceneManager* mSmgr;
		irr::video::IVideoDriver* mDriver;

		CameraHandle mViewCamera;
		CameraHandle mCullCamera;

		static const char * DEFAULTTITLE;
		static const math::vec2 DEFAULTSIZE;
		static const GLWindow::DisplayType DEFAULTDISPLAYTYPE;
		static const GLWindow::SwapMode DEFAULTSWAPMODE;
	};
}

