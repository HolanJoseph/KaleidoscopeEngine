#pragma once

#include <Synchronization/Locks/Semaphore/Semaphore.h>

#include <Math/Math.h>

#include <irrlicht.h>

#include <list>

namespace kaleidoscope
{
	class IrrlichtController
	{
	public:
		IrrlichtController();
		~IrrlichtController();

		bool createDevice(irr::SIrrlichtCreationParameters& params);
		void closeDevice();

		irr::video::ITexture*      getTexture(const irr::io::path& p);
		irr::scene::IMesh* getMesh(const irr::io::path& p);

		irr::scene::ISceneNode*		  addSceneNode();
		irr::scene::IMeshSceneNode*   addMeshSceneNode();
		irr::scene::ICameraSceneNode* addCameraSceneNode();
		irr::scene::ILightSceneNode*  addLightSceneNode();

		void removeSceneNode(irr::scene::ISceneNode* node);

		void drawAll();

		bool mipMapsEnabled() const;
		bool normalizedNormalsEnabled() const;
		bool backfaceCullingEnabled() const;
		bool frontfaceCullingEnabled() const;
		U8   getAntiAliasingMode() const;
		bool fogEnabled() const;
		bool lightingEnabled() const;
		bool ZWriteEnabled() const;

		void enableMipMaps(bool value);
		void enableNormalizedNormals(bool value);
		void enableBackfaceCulling(bool value);
		void enableFrontfaceCulling(bool value);
		void setAntiAliasingMode(U8 mode);
		void enableFog(bool value);
		void enableLighting(bool value);
		void enableZWrite(bool value);

	private:
		irr::IrrlichtDevice* mDevice;
		irr::scene::ISceneManager* mSmgr;
		irr::video::IVideoDriver* mDriver;
		kaleidoscope::Semaphore useSem;

		std::list<irr::scene::ISceneNode*> mAllSceneNodes;

		bool useMipMaps;
		bool normalizeNormals;
		bool backfaceCulling;
		bool frontfaceCulling;
		U8 antiAliasingMode;
		bool isFogEnabled;
		bool lighting;
		bool zWriteEnabled;


	};
}

