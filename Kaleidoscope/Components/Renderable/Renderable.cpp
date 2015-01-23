#include <Components/Renderable/Renderable.h>

#include <Components/Renderable/RenderableHandle.h>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;

#include <Synchronization/Locks/Semaphore/Semaphore.h>


#include <string>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <boost/foreach.hpp>
#include <Utility/Parsing/parseMathsFromStrings.h>

namespace kaleidoscope
{
	static kaleidoscope::Semaphore createSem;
	static kaleidoscope::Semaphore nameSem;
	static kaleidoscope::Semaphore errorSem;

	Renderable::Renderable()
	{
		mInitialized = false;
		mNextInFreeList = NULL;
	}


	/*
	* bool kaleidoscope::Renderable::init(const kaleidoscope::StringID name, kaleidoscope::TransformHandle th)
	*
	* In: StringID : The internal unique name for the renderable.
	* In: TransformHandle : The transform to be associated with this renderable, representing its location.
	* Out: bool : true on success.
	*			  false on failure.
	*
	* Used internally by Renderable::Create() to initialize a pool object.
	*/
	bool Renderable::init(const StringID name, TransformHandle th)
	{
		mInitialized = true;

		mName = name;
		mPoolIndex = this - &sRenderablePool[0];

		mirrMesh = mIrrController->addMeshSceneNode();
		if (mirrMesh == NULL)
		{
			setError(CODE_IRRLICHT_ERROR, "Could not add a scene node to Irrlicht Scene Manager");
			return false;
		}


		mMeshPath = 0;
		mAlbedoPath = 0;

		mTransform = th;

		return true;
	}


	/*
	* bool kaleidoscope::Renderable::destroy()
	*
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used internally by Renderable::Destroy() to reset a pool objects state so it may be reused.
	*/
	bool Renderable::destroy()
	{
		mIrrController->removeSceneNode(mirrMesh);

		mMeshPath = 0;
		mAlbedoPath = 0;

		mTransform = TransformHandle::null;

		mInitialized = false;
		mNextInFreeList = NULL;

		return true;
	}

	Renderable::~Renderable(){}


	/*
	* void kaleidoscope::Renderable::SerializeIn(const boost::property_tree::ptree& renderableInfo)
	*
	* In: ptree : property tree representing the renderables internal state.
	* Out: void :
	*
	* Fill in the renderables state with the values provided by the property tree.
	*
	* <mesh>path/to/mesh</mesh>
	* <material-list>
	*	<material>
	*		<shader>diffuse</shader>
	*		<albedo>path/to/albedotexture</albedo>
	*		<normalMap>path/to/normalmaptexture</normalMap>
	*		<shininess>number</shininess>
	*		<specularColor>(r, g, b, a)</specularColor>
	*	<material>
	* </material-list>
	*/
	void Renderable::SerializeIn(const boost::property_tree::ptree& renderableInfo)
	{
		using boost::property_tree::ptree;

		boost::optional<std::string> mesh = renderableInfo.get_optional<std::string>("mesh");
		boost::optional<const ptree&> matList = renderableInfo.get_child_optional("material-list");

		if (mesh)
		{
			setMesh(internString((*mesh).c_str()));
		}

		if (matList)
		{
			SerializeMaterialListIn(*matList);
		}


	}


	/*
	* void kaleidoscope::Renderable::SerializeMaterialListIn(const boost::property_tree::ptree& matList)
	*
	* In: ptree : property tree representing the renderables materials internal state.
	* Out: void :
	*
	* Used by SerializeIn() to initialize its materials.
	*/
	void Renderable::SerializeMaterialListIn(const boost::property_tree::ptree& matList)
	{
		using boost::property_tree::ptree;

		U32 i = 0;
		BOOST_FOREACH(ptree::value_type const& mat, matList)
		{
			if (mat.first.compare("material") == 0)
			{
				SerializeMaterialIn(mat.second, i++);
			}
		}
	}


	/*
	* void kaleidoscope::Renderable::SerializeMaterialIn(const boost::property_tree::ptree& mat, U32 matNum)
	*
	* In: ptree : property tree representing the materials internal state.
	* In: U32 : The number of the material to initialize.
	* Out: void :
	*
	* Used by SerializeMaterialListIn() to initialize a material.
	*/
	void Renderable::SerializeMaterialIn(const boost::property_tree::ptree& mat, U32 matNum)
	{
		if (matNum >= numMaterials())
		{
			return;
		}

		boost::optional<std::string> shader = mat.get_optional<std::string>("shader");
		boost::optional<std::string> albedoPath = mat.get_optional<std::string>("albedo");
		boost::optional<std::string> normalMapPath = mat.get_optional<std::string>("normalMap");
		boost::optional<F32> shininess = mat.get_optional<F32>("shininess");
		boost::optional<std::string> specularColor = mat.get_optional<std::string>("specularColor");

		if (shader)
		{
			setMaterialShader(matNum, internString(shader->c_str()));
		}
		if (albedoPath)
		{
			setMaterialAlbedo(matNum, internString(albedoPath->c_str()));
		}
		if (normalMapPath)
		{
			setMaterialNormalMap(matNum, internString(normalMapPath->c_str()));
		}
		if (shininess)
		{
			setMaterialShininess(matNum, *shininess);
		}
		if (specularColor)
		{
			bool success = false;
			kaleidoscope::math::vec4 spec = stringToVec4(specularColor->c_str(), success);
			if (success)
			{
				setMaterialSpecularColor(matNum, spec);
			}
		}
	}


	/*
	* boost::property_tree::ptree* kaleidoscope::Renderable::SerializeOut()
	*
	* In: void :
	* Out: ptree* : a pointer to a ptree representing the internal state of the renderable.
	*
	* THE USER IS RESPONSIBILE FOR DELETING THE POINTER.
	*/
	boost::property_tree::ptree* Renderable::SerializeOut()
	{
		using boost::property_tree::ptree;

		ptree* rI = new ptree();

		// SERIALIZE
		if (mMeshPath != 0)
		{
			rI->add("mesh", getString(mMeshPath));
		}

		if (mAlbedoPath != 0)
		{
			rI->add("material.albedo", getString(mAlbedoPath));
		}

		return rI;
	}


	/*
	* void kaleidoscope::Renderable::setMesh(kaleidoscope::StringID mesh)
	*
	* In: StringID : The path to the mesh to set.
	* Out: void :
	*/
	void Renderable::setMesh(StringID mesh)
	{
		irr::scene::IMesh* m = mIrrController->getMesh(getString(mesh));
		mirrMesh->setMesh(m);

		if (m != NULL)
		{
			mMeshPath = mesh;
		}
	}


	/*
	* void kaleidoscope::Renderable::removeMesh()
	*
	* In: void :
	* Out: void :
	*
	* Remove the mesh that this renderable is currently associated with/ displaying.
	*/
	void Renderable::removeMesh()
	{
		mIrrController->removeSceneNode(mirrMesh);
		mirrMesh = mIrrController->addMeshSceneNode();


		mMeshPath = 0;
		mAlbedoPath = 0;
	}


	/*
	* bool kaleidoscope::Renderable::visible() const
	*
	* In: void :
	* Out: bool : true if the renderable is visible.
	*			  false if the renderable is invisible.
	*/
	bool Renderable::visible() const
	{
		return mirrMesh->isVisible();
	}


	/*
	* void kaleidoscope::Renderable::makeVisible()
	*
	* In: void :
	* Out: void :
	*
	* set it so the renderables mesh is drawn.
	*/
	void Renderable::makeVisible()
	{
		mirrMesh->setVisible(true);
	}


	/*
	* void kaleidoscope::Renderable::makeInvisible()
	*
	* In: void :
	* Out: void :
	*
	* set it so the renderables mesh is not drawn.
	*/
	void Renderable::makeInvisible()
	{
		mirrMesh->setVisible(false);
	}


	/*
	* U32 kaleidoscope::Renderable::numMaterials() const
	*
	* In: void :
	* Out: U32 : The number of materials this renderable has.
	*/
	U32 Renderable::numMaterials() const
	{
		return mirrMesh->getMaterialCount();
	}


	/*
	* void kaleidoscope::Renderable::setMaterialShader(U32 matNumber, kaleidoscope::StringID shader)
	*
	* In: U32 : The number of the material to change.
	* In: StringID : The name of the shader to use for this material.
	* Out: void :
	*
	*     "diffuse"   - diffuse material albedo only, Gouraud Shading only.
	* "normal mapped" - albedo, and normal map, per pixel lighting.
	*/
	void Renderable::setMaterialShader(U32 matNumber, StringID shader)
	{
		if (shader == internString("diffuse"))
		{
			mirrMesh->getMaterial(matNumber).MaterialType = irr::video::EMT_SOLID;
		}
		else if (shader == internString("normal mapped"))
		{
			mirrMesh->getMaterial(matNumber).MaterialType = irr::video::EMT_NORMAL_MAP_SOLID;
		}
	}


	/*
	* kaleidoscope::StringID kaleidoscope::Renderable::getMaterialShader(U32 matNumber) const
	*
	* In: U32 : The number of the material to change.
	* Out: StringID : The name of the shader used to draw the renderables material.
	*/
	StringID Renderable::getMaterialShader(U32 matNumber) const
	{
		irr::video::E_MATERIAL_TYPE shader = mirrMesh->getMaterial(matNumber).MaterialType;

		if (shader == irr::video::EMT_SOLID)
		{
			return internString("diffuse");
		}
		else if (shader == irr::video::EMT_NORMAL_MAP_SOLID)
		{
			return internString("normal mapped");
		}

		return 0;
	}


	/*
	* void kaleidoscope::Renderable::setMaterialAlbedo(U32 matNumber, kaleidoscope::StringID path)
	*
	* In: U32 : The number of the material to change.
	* In: StringID : The path to the albedo texture.
	* Out: void :
	*/
	void Renderable::setMaterialAlbedo(U32 matNumber, StringID path)
	{
		mirrMesh->getMaterial(matNumber).setTexture(0, mIrrController->getTexture(getString(path)));
	}


	/*
	* void kaleidoscope::Renderable::setMaterialNormalMap(U32 matNumber, kaleidoscope::StringID path)
	*
	* In: U32 : The number of the material to change.
	* In: StringID : The path to the normal map texture.
	* Out: void :
	*/
	void Renderable::setMaterialNormalMap(U32 matNumber, StringID path)
	{
		mirrMesh->getMaterial(matNumber).setTexture(1, mIrrController->getTexture(getString(path)));
	}


	/*
	* void kaleidoscope::Renderable::setMaterialShininess(U32 matNumber, F32 val)
	*
	* In: U32 : The number of the material to change.
	* In: F32 : The value to set the materials shininess too.
	* Out: void :
	*/
	void Renderable::setMaterialShininess(U32 matNumber, F32 val)
	{
		mirrMesh->getMaterial(matNumber).Shininess = val;
	}


	/*
	* F32 kaleidoscope::Renderable::getMaterialShininess(U32 matNumber) const
	*
	* In: U32 : The number of the material to query.
	* Out: F32 : The current value of the materials shininess.
	*/
	F32 Renderable::getMaterialShininess(U32 matNumber) const
	{
		return mirrMesh->getMaterial(matNumber).Shininess;
	}


	/*
	* void kaleidoscope::Renderable::setMaterialSpecularColor(U32 matNumber, const kaleidoscope::math::vec4& c)
	*
	* In: U32 : The number of the material to change.
	* In: vec4 : The new color to use as the materials specular color.
	* Out: void :
	*
	* COLOR IS REPRESENTED WITH VALUES BETWEEN 0-255.
	*/
	void Renderable::setMaterialSpecularColor(U32 matNumber, const math::vec4& c)
	{
		math::ivec4 ic = static_cast<math::ivec4>(c);
		mirrMesh->getMaterial(matNumber).SpecularColor = irr::video::SColor(ic.w, ic.x, ic.y, ic.z);
	}


	/*
	* kaleidoscope::math::vec4 kaleidoscope::Renderable::getMaterialSpecularColor(U32 matNumber) const
	*
	* In: U32 : The number of the material to query.
	* Out: vec4 : The materials current specular color.
	*
	* COLOR IS REPRESENTED WITH VALUES BETWEEN 0-255.
	*/
	math::vec4 Renderable::getMaterialSpecularColor(U32 matNumber) const
	{
		irr::video::SColor sc = mirrMesh->getMaterial(matNumber).SpecularColor;
		return math::vec4(sc.getRed(), sc.getGreen(), sc.getBlue(), sc.getAlpha());
	}








	/*
	* void kaleidoscope::Renderable::setAlbedo(kaleidoscope::StringID albedoTex)
	*
	* In: ----- :
	* Out: ---- :
	*
	* DEPRICATED REPLACED BY setMaterialAlbedo()
	*/
	void Renderable::setAlbedo(StringID albedoTex)
	{
		irr::video::ITexture* tex = mIrrController->getTexture(getString(albedoTex));

		if (tex != NULL)
		{
			mAlbedoPath = albedoTex;
		}

		mirrMesh->setMaterialTexture(0, tex);
	}


	/*
	* void kaleidoscope::Renderable::printState() const
	*
	* In:  void :
	* Out: void :
	*
	* Print the internal state of the transform
	* NEEDS TO BE REPLACED
	*	replace with CSV
	*/
	void Renderable::printState() const
	{
		gLogManager.log("		name = %s", getString(mName));
		gLogManager.log("		mPoolIndex = %u", mPoolIndex);
		(mMeshPath != 0 ? gLogManager.log("		mesh = %s", getString(mMeshPath)) : NULL);
		(mAlbedoPath != 0 ? gLogManager.log("		albedo = %s", getString(mAlbedoPath)) : NULL);
	}


	/*
	* bool kaleidoscope::Renderable::StartUp(kaleidoscope::IrrlichtController* irrControl, const boost::property_tree::ptree& properties)
	*
	* In: ptree : The properties used to start up the renderable subsystem
	* Out: bool : true on success.
	*			  false on failure.
	*
	* objects = U32 The maximum number of renderables that can be allocated.
	*/
	bool Renderable::StartUp(IrrlichtController* irrControl, const boost::property_tree::ptree& properties)
	{
		if (!initialized)
		{
			initialized = true;

			if (!StringIDInit())
			{
				initialized = false;
				return false;
			}

			boost::optional<U32> numObjects = properties.get_optional<U32>("objects");
			if (numObjects)
			{
				MAXNUMOBJECTS = *numObjects;
			}
			else
			{
				MAXNUMOBJECTS = DEFAULTMAX;
			}

			sRenderablePool = new Renderable[MAXNUMOBJECTS];

			mErrorManager = ErrorManager();

			I32 success = Semaphore::Semaphore_init(&createSem, 1);
			if (success != 0)
			{
				initialized = false;
				return false;
			}

			success = Semaphore::Semaphore_init(&nameSem, 1);
			if (success != 0)
			{
				initialized = false;
				return false;
			}

			success = Semaphore::Semaphore_init(&errorSem, 1);
			if (success != 0)
			{
				initialized = false;
				return false;
			}

			if (irrControl == NULL)
			{
				initialized = false;
				return false;
			}
			mIrrController = irrControl;

			sFirstFree = &sRenderablePool[0];

			for (U32 i = 0; i < MAXNUMOBJECTS - 1; ++i)
			{
				sRenderablePool[i].mNextInFreeList = &sRenderablePool[i + 1];
			}

			sRenderablePool[MAXNUMOBJECTS - 1].mNextInFreeList = NULL;

			return true;
		}

		return true;
	}


	/*
	* bool kaleidoscope::Renderable::ShutDown()
	*
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used to shutdown the renderable subsystem.
	*/
	bool Renderable::ShutDown()
	{
		if (initialized)
		{
			initialized = false;

			delete[] sRenderablePool;

			Semaphore::Semaphore_destroy(&createSem);
			Semaphore::Semaphore_destroy(&nameSem);
			Semaphore::Semaphore_destroy(&errorSem);
			
			return true;
		}

		return true;
	}


	/*
	* kaleidoscope::RenderableHandle kaleidoscope::Renderable::Create(const kaleidoscope::TransformHandle& th)
	*
	* In: TransformHandle : The transform to associate with the new renderable.
	* Out: RenderableHandle : An freshly initialized renderable.
	*						  RenderableHandle::null if the initilization failed.
	*
	* Error handling functions can be used to get the reason the creation failed.
	*/
	RenderableHandle Renderable::Create(const TransformHandle& th)
	{
		Semaphore::Semaphore_wait(&createSem);

		if (sFirstFree != NULL)
		{
			Renderable* const newRend = sFirstFree;
			sFirstFree = newRend->mNextInFreeList;
			bool b = newRend->init(GenerateName(), th);

			// Handle init failure.
			if (b == false)
			{
				newRend->destroy();

				U32 i = (newRend - &sRenderablePool[0]);
				sRenderablePool[i].mNextInFreeList = sFirstFree;
				sFirstFree = &sRenderablePool[i];

				Semaphore::Semaphore_post(&createSem);
				return RenderableHandle::null;
			}

			RenderableHandle rh(newRend);
			Semaphore::Semaphore_post(&createSem);
			return rh;
		}
		else
		{
			setError(CODE_POOL_FULL, "The maximum number of renderables has been allocated, increase the number of renderables specified in properties at StartUp");
			Semaphore::Semaphore_post(&createSem);
			return RenderableHandle::null;
		}
	}


	/*
	* kaleidoscope::RenderableHandle kaleidoscope::Renderable::Create(const kaleidoscope::TransformHandle& th, const boost::property_tree::ptree& renderableInfo)
	*
	* In: TransformHandle : The transform to associate with the new renderable.
	* In: ptree : The properties to initialize the transform with.
	* Out: RenderableHandle : An initialized renderable.
	*						  RenderableHandle::null if the initilization failed.
	*
	* Error handling functions can be used to get the reason the creation failed.
	*/
	RenderableHandle Renderable::Create(const TransformHandle& th, const boost::property_tree::ptree& renderableInfo)
	{
		RenderableHandle rh = Renderable::Create(th);
		if (rh != RenderableHandle::null)
		{
			rh.getObject()->SerializeIn(renderableInfo);
		}
		return rh;
	}


	/*
	* void kaleidoscope::Renderable::Destroy(const kaleidoscope::RenderableHandle& rh)
	*
	* In: RenderableHandle : The renderable to destroy.
	* Out: void :
	*/
	void Renderable::Destroy(const RenderableHandle& rh)
	{
		Semaphore::Semaphore_wait(&createSem);

		Renderable* const r = rh.getObject();

		if (rh.valid())
		{
			U32 i = (r - &sRenderablePool[0]);
			sRenderablePool[i].destroy();
			sRenderablePool[i].mNextInFreeList = sFirstFree;
			sFirstFree = &sRenderablePool[i];
		}

		Semaphore::Semaphore_post(&createSem);
	}


	/*
	* void kaleidoscope::Renderable::UpdateAll()
	*
	* In: void :
	* Out: void :
	*
	* Synch the underlying Irrlicht State with the renderables state for rendering.
	* Internal use by RenderManager.
	*/
	void Renderable::UpdateAll()
	{
		for (U32 i = 0; i < MAXNUMOBJECTS; ++i)
		{
			if (sRenderablePool[i].mInitialized)
			{
				Renderable* r = &(sRenderablePool[i]);

				math::vec3 wpos = r->transform().getWorldPosition();
				irr::core::vector3df pos(wpos.x, wpos.y, wpos.z);
				r->mirrMesh->setPosition(pos);
				r->mirrMesh->updateAbsolutePosition();

				math::quat wori = r->transform().getWorldOrientation();
				math::vec3 weuler = kmath::eulerAnglesNoGimalProtect(wori);
				irr::core::vector3df rot(weuler.x, weuler.y, weuler.z);
				rot = rot * (180.0f / kmath::PI );
				r->mirrMesh->setRotation(rot);
				r->mirrMesh->updateAbsolutePosition();

				math::vec3 wscale = r->transform().getWorldScale();
				irr::core::vector3df scl(wscale.x, wscale.y, wscale.z);
				r->mirrMesh->setScale(scl);
				r->mirrMesh->updateAbsolutePosition();

			}
		}
	}


	/*
	* bool kaleidoscope::Renderable::hasPendingError()
	*
	* In: void :
	* Out: bool : true if there is an error.
	*			  false if there isnt an error.
	*
	* If Create throws an error the system logs an error. The error state stays the same until it is either cleared by
	*	the user or Create throws another error. In this case the previous error is overwritten.
	*/
	bool Renderable::hasPendingError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		bool b = mErrorManager.hasPendingError();
		Semaphore::Semaphore_post(&errorSem);
		return b;
	}


	/*
	* void kaleidoscope::Renderable::clearError()
	*
	* In: void :
	* Out: void :
	*
	* If there was an error in the transform system clear it out.
	* Used after processing an error to tell when the next error occurs.
	*/
	void Renderable::clearError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.clearError();
		Semaphore::Semaphore_post(&errorSem);
	}


	/*
	* kaleidoscope::ErrorCode kaleidoscope::Renderable::getErrorCode()
	*
	* In: void :
	* Out: ErrorCode : The error code representing the current state of the transform system.
	*/
	ErrorCode Renderable::getErrorCode()
	{
		Semaphore::Semaphore_wait(&errorSem);
		ErrorCode ec = mErrorManager.getErrorCode();
		Semaphore::Semaphore_post(&errorSem);
		return ec;
	}


	/*
	* std::string kaleidoscope::Renderable::getErrorString()
	*
	* In: void :
	* Out: ErrorCode : The detailed string explaining why the error happened.
	*/
	std::string Renderable::getErrorString()
	{
		Semaphore::Semaphore_wait(&errorSem);
		std::string es = mErrorManager.getErrorString();
		Semaphore::Semaphore_post(&errorSem);
		return es;
	}


	/*
	* void kaleidoscope::Renderable::setError(kaleidoscope::ErrorCode ec, const std::string& es)
	*
	* In: ErrorCode : What error happened in the system.
	* In: string : The detailed description of what happened.
	* Out: void :
	*
	* used internally.
	*/
	void Renderable::setError(ErrorCode ec, const std::string& es)
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.setError(ec, es);
		Semaphore::Semaphore_post(&errorSem);
	}


	/*
	* kaleidoscope::StringID kaleidoscope::Renderable::GenerateName()
	*
	* In: void :
	* Out: StringID : The name generated for the next renderable.
	*
	* Used internally to make sure each renderable has a unique name.
	*/
	StringID Renderable::GenerateName()
	{
		char buff[25];
		Semaphore::Semaphore_wait(&nameSem);
		I32 n = sprintf_s(buff, "Renderable__%u", numRenderables++);
		Semaphore::Semaphore_post(&nameSem);
		StringID name = internString(buff);
		return name;
	}


	const char * Renderable::LUA_TYPE_NAME = "kaleidoscope.renderable";
	StringID Renderable::NAME = hashCRC32("renderable");
	U32 Renderable::MAXNUMOBJECTS = 0;



	bool Renderable::initialized = false;

	U32 Renderable::DEFAULTMAX = 10;

	U32 Renderable::numRenderables = 0;
	Renderable* Renderable::sFirstFree = NULL;
	Renderable* Renderable::sRenderablePool = NULL;

	IrrlichtController* Renderable::mIrrController = NULL;

	ErrorManager Renderable::mErrorManager;
}