#include <Rendering/Irrlicht/IrrlichtController.h>

#include <algorithm>

namespace kaleidoscope
{

	/*
	* kaleidoscope::IrrlichtController::IrrlichtController()
	*
	* Set the defaults for the Irrlicht Controller.
	*/
	IrrlichtController::IrrlichtController()
	{
		mDevice = NULL;
		mSmgr = NULL;
		mDevice = NULL;

		mAllSceneNodes = std::list<irr::scene::ISceneNode*>();

		useMipMaps = true;
		normalizeNormals = true;
		backfaceCulling = true;
		frontfaceCulling = false;
		antiAliasingMode = irr::video::EAAM_SIMPLE | irr::video::EAAM_LINE_SMOOTH;
		isFogEnabled = false;
		lighting = false;
		zWriteEnabled = true;
	}


	IrrlichtController::~IrrlichtController()
	{

	}


	/*
	* bool kaleidoscope::IrrlichtController::createDevice(irr::SIrrlichtCreationParameters& params)
	*
	* In: SIrrlichtCreationParameters& params : parameters to initialize Irrlicht with.
	* Out: bool : true on success.
	*			  false on failure.
	*
	* Create and initialize all of the state Irrlicht needs.
	* Should be called on every IrrlichtController created.
	*/
	bool IrrlichtController::createDevice(irr::SIrrlichtCreationParameters& params)
	{
		mDevice = irr::createDeviceEx(params);
		if (mDevice == NULL)
		{
			return false;
		}
		mSmgr = mDevice->getSceneManager();
		mDriver = mDevice->getVideoDriver();

		mDriver->setTextureCreationFlag(irr::video::ETCF_ALWAYS_32_BIT, true);

		if (Semaphore::Semaphore_init(&useSem, 1) != 0)
		{
			return false;
		}


		mSmgr->addSkyBoxSceneNode(
			mDriver->getTexture("media/irrlicht2_up.jpg"),
			mDriver->getTexture("media/irrlicht2_dn.jpg"),
			mDriver->getTexture("media/irrlicht2_lf.jpg"),
			mDriver->getTexture("media/irrlicht2_rt.jpg"),
			mDriver->getTexture("media/irrlicht2_ft.jpg"),
			mDriver->getTexture("media/irrlicht2_bk.jpg"));

		mSmgr->addLightSceneNode(NULL, irr::core::vector3df(0, 2, 0));


		return true;
	}


	/*
	* void kaleidoscope::IrrlichtController::closeDevice()
	*
	* Release all Irrlicht State.
	* Must be called before the destructor is called.
	*/
	void IrrlichtController::closeDevice()
	{
		mDevice->closeDevice();
		mDevice->drop();

		Semaphore::Semaphore_destroy(&useSem);
	}

	/*
	* irr::video::ITexture* kaleidoscope::IrrlichtController::getTexture(const irr::io::path& p)
	*
	* In: irr::io::path& : The path to the texture file.
	* Out: irr::video::ITexture* : A pointer to the loaded texture data if the file existed.
	*							   NULL pointer if the file doesn't exist.
	*/
	irr::video::ITexture* IrrlichtController::getTexture(const irr::io::path& p)
	{
		Semaphore::Semaphore_wait(&useSem);
		irr::video::ITexture* tex = mDriver->getTexture(p);
		Semaphore::Semaphore_post(&useSem);
		return tex;
	}


	/*
	* irr::video::ITexture* kaleidoscope::IrrlichtController::getMesh(const irr::io::path& p)
	*
	* In: irr::io::path& : The path to the mesh file.
	* Out: irr::video::ITexture* : A pointer to the loaded mesh data if the file existed.
	*							   NULL pointer if the file doesn't exist.
	*/
	irr::scene::IMesh* IrrlichtController::getMesh(const irr::io::path& p)
	{
		Semaphore::Semaphore_wait(&useSem);
		irr::scene::IAnimatedMesh* mesh = mSmgr->getMesh(p);

		if (mesh == NULL)
		{
			return NULL;
		}

		irr::scene::IMesh* tanMesh = mSmgr->getMeshManipulator()->createMeshWithTangents(mesh->getMesh(0));


		tanMesh->setMaterialFlag(irr::video::EMF_USE_MIP_MAPS, useMipMaps);
		tanMesh->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, normalizeNormals);
		tanMesh->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, backfaceCulling);
		tanMesh->setMaterialFlag(irr::video::EMF_FRONT_FACE_CULLING, frontfaceCulling);
		tanMesh->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, static_cast<bool>(antiAliasingMode));
		tanMesh->setMaterialFlag(irr::video::EMF_FOG_ENABLE, isFogEnabled);
		tanMesh->setMaterialFlag(irr::video::EMF_LIGHTING, lighting);
		tanMesh->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, zWriteEnabled);
		tanMesh->setMaterialFlag(irr::video::EMF_ANISOTROPIC_FILTER, true);
		tanMesh->setMaterialFlag(irr::video::EMF_TRILINEAR_FILTER, true);

		Semaphore::Semaphore_post(&useSem);
		return tanMesh;
	}


	/*
	* irr::scene::ISceneNode* kaleidoscope::IrrlichtController::addSceneNode()
	*
	* In: void :
	* Out: ISceneNode* : A pointer to an Irrlicht Scene Node.
	*
	* Adds a scene node to the Irrlicht scene.
	*/
	irr::scene::ISceneNode* IrrlichtController::addSceneNode()
	{
		Semaphore::Semaphore_wait(&useSem);
		irr::scene::ISceneNode* s = mSmgr->addEmptySceneNode();
		mAllSceneNodes.push_back(s);
		Semaphore::Semaphore_post(&useSem);
		return s;
	}


	/*
	* irr::scene::IMeshSceneNode* kaleidoscope::IrrlichtController::addMeshSceneNode()
	*
	* In: void :
	* Out: IMeshSceneNode* : A pointer to an Irrlicht Mesh Scene Node.
	*
	* Adds a mesh scene node to the Irrlicht scene.
	*/
	irr::scene::IMeshSceneNode*   IrrlichtController::addMeshSceneNode()
	{
		Semaphore::Semaphore_wait(&useSem);
		irr::scene::IMeshSceneNode* m = mSmgr->addMeshSceneNode(NULL, NULL, -1, irr::core::vector3df(0.0f, 0.0f, 0.0f), irr::core::vector3df(0.0f, 0.0f, 0.0f), irr::core::vector3df(1.0f, 1.0f, 1.0f), true);
		mAllSceneNodes.push_back(m);
		Semaphore::Semaphore_post(&useSem);
		return m;
	}


	/*
	* irr::scene::ICameraSceneNode* kaleidoscope::IrrlichtController::addCameraSceneNode()
	*
	* In: void :
	* Out: ICameraSceneNode* : A pointer to an Irrlicht Camera Scene Node.
	*
	* Adds a camera scene node to the Irrlicht scene.
	*/
	irr::scene::ICameraSceneNode* IrrlichtController::addCameraSceneNode()
	{
		Semaphore::Semaphore_wait(&useSem);
		irr::scene::ICameraSceneNode* c = mSmgr->addCameraSceneNode();
		mAllSceneNodes.push_back(c);
		Semaphore::Semaphore_post(&useSem);
		return c;
	}


	/*
	* irr::scene::ILightSceneNode* kaleidoscope::IrrlichtController::addLightSceneNode()
	*
	* In: void :
	* Out: ILightSceneNode* : A pointer to an Irrlicht Light Scene Node.
	*
	* Adds a light scene node to the Irrlicht scene.
	*/
	irr::scene::ILightSceneNode*  IrrlichtController::addLightSceneNode()
	{
		Semaphore::Semaphore_wait(&useSem);
		irr::scene::ILightSceneNode* l = mSmgr->addLightSceneNode();
		mAllSceneNodes.push_back(l);
		Semaphore::Semaphore_post(&useSem);
		return l;
	}


	/*
	* void kaleidoscope::IrrlichtController::removeSceneNode(irr::scene::ISceneNode* node)
	*
	* In: ISceneNode* : A pointer to the scene node to remove.
	* Out: void :
	* 
	* Removes the specified scene node from the Irrlicht Scene.
	*/
	void IrrlichtController::removeSceneNode(irr::scene::ISceneNode* node)
	{
		if (node != NULL)
		{
			Semaphore::Semaphore_wait(&useSem);
			std::list<irr::scene::ISceneNode*>::iterator loc = std::find(mAllSceneNodes.begin(), mAllSceneNodes.end(), node);
			mAllSceneNodes.erase(loc);
			node->remove();
			Semaphore::Semaphore_post(&useSem);
		}
	}


	/*
	* void kaleidoscope::IrrlichtController::drawAll()
	*
	* In: void :
	* Out: void :
	*
	* Draw the Irrlicht Scene.
	*/
	void IrrlichtController::drawAll()
	{
		mDevice->getTimer()->tick();

		// draw engine picture
		mDriver->beginScene(true, true, 0);
		mSmgr->drawAll();
		mDriver->endScene();
	}

	/*
	* Check if the specified property in enabled or disabled .
	*/
	bool IrrlichtController::mipMapsEnabled() const { return useMipMaps; }
	bool IrrlichtController::normalizedNormalsEnabled() const { return normalizeNormals; }
	bool IrrlichtController::backfaceCullingEnabled() const { return backfaceCulling; }
	bool IrrlichtController::frontfaceCullingEnabled() const { return frontfaceCulling; }
	U8   IrrlichtController::getAntiAliasingMode() const { return antiAliasingMode; }
	bool IrrlichtController::fogEnabled() const { return isFogEnabled; }
	bool IrrlichtController::lightingEnabled() const { return lighting; }
	bool IrrlichtController::ZWriteEnabled() const { return zWriteEnabled; }

	
	/*
	* void kaleidoscope::IrrlichtController::enableXXXXXXX(bool value)
	*
	* In : bool value : whether or not the specified property should be enabled or disabled.
	*
	* This applied for all functions of this form.
	*/
	void IrrlichtController::enableMipMaps(bool value)
	{
		if (value == useMipMaps)
		{
			return;
		}

		useMipMaps = value;

		for (std::list<irr::scene::ISceneNode*>::iterator node = mAllSceneNodes.begin(); node != mAllSceneNodes.end(); ++node)
		{
			(*node)->setMaterialFlag(irr::video::EMF_USE_MIP_MAPS, useMipMaps);
		}
	}


	void IrrlichtController::enableNormalizedNormals(bool value)
	{
		if (value == normalizeNormals)
		{
			return;
		}

		normalizeNormals = value;

		for (std::list<irr::scene::ISceneNode*>::iterator node = mAllSceneNodes.begin(); node != mAllSceneNodes.end(); ++node)
		{
			(*node)->setMaterialFlag(irr::video::EMF_NORMALIZE_NORMALS, normalizeNormals);
		}
	}


	void IrrlichtController::enableBackfaceCulling(bool value)
	{
		if (value == backfaceCulling)
		{
			return;
		}

		backfaceCulling = value;

		for (std::list<irr::scene::ISceneNode*>::iterator node = mAllSceneNodes.begin(); node != mAllSceneNodes.end(); ++node)
		{
			(*node)->setMaterialFlag(irr::video::EMF_BACK_FACE_CULLING, backfaceCulling);
		}
	}


	void IrrlichtController::enableFrontfaceCulling(bool value)
	{
		if (value == frontfaceCulling)
		{
			return;
		}

		frontfaceCulling = value;

		for (std::list<irr::scene::ISceneNode*>::iterator node = mAllSceneNodes.begin(); node != mAllSceneNodes.end(); ++node)
		{
			(*node)->setMaterialFlag(irr::video::EMF_FRONT_FACE_CULLING, frontfaceCulling);
		}
	}


	void IrrlichtController::setAntiAliasingMode(U8 mode)
	{
		if (mode == antiAliasingMode)
		{
			return;
		}

		antiAliasingMode = mode;

		for (std::list<irr::scene::ISceneNode*>::iterator node = mAllSceneNodes.begin(); node != mAllSceneNodes.end(); ++node)
		{
			(*node)->setMaterialFlag(irr::video::EMF_ANTI_ALIASING, static_cast<bool>(antiAliasingMode));
		}
	}


	void IrrlichtController::enableFog(bool value)
	{
		if (value == isFogEnabled)
		{
			return;
		}

		isFogEnabled = value;

		for (std::list<irr::scene::ISceneNode*>::iterator node = mAllSceneNodes.begin(); node != mAllSceneNodes.end(); ++node)
		{
			(*node)->setMaterialFlag(irr::video::EMF_FOG_ENABLE, isFogEnabled);
		}
	}


	void IrrlichtController::enableLighting(bool value)
	{
		if (value == lighting)
		{
			return;
		}

		lighting = value;

		for (std::list<irr::scene::ISceneNode*>::iterator node = mAllSceneNodes.begin(); node != mAllSceneNodes.end(); ++node)
		{
			(*node)->setMaterialFlag(irr::video::EMF_LIGHTING, lighting);
		}
	}


	void IrrlichtController::enableZWrite(bool value)
	{
		if (value == zWriteEnabled)
		{
			return;
		}

		zWriteEnabled = value;

		for (std::list<irr::scene::ISceneNode*>::iterator node = mAllSceneNodes.begin(); node != mAllSceneNodes.end(); ++node)
		{
			(*node)->setMaterialFlag(irr::video::EMF_ZWRITE_ENABLE, zWriteEnabled);
		}
	}


}