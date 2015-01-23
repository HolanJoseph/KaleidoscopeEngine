#include <Components/Camera/Camera.h>

#include <Components/Camera/CameraHandle.h>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;

#include <Synchronization/Locks/Semaphore/Semaphore.h>

#include <string>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>

namespace kaleidoscope
{
	
	static kaleidoscope::Semaphore createSem;
	static kaleidoscope::Semaphore nameSem;
	static kaleidoscope::Semaphore errorSem;

	Camera::Camera()
	{
		mInitialized = false;
		mNextInFreeList = NULL;
	}


	/*
	* bool kaleidoscope::Camera::init(const kaleidoscope::StringID name, kaleidoscope::TransformHandle th)
	*
	* In: StringID : The internal unique name for the camera.
	* In: TransformHandle : The transform to be associated with this renderable, representing its location.
	* Out: bool : true on success.
	*			  false on failure.
	*
	* Used internally by Camera::Create() to initialize a pool object.
	*/
	bool Camera::init(const StringID name, TransformHandle th)
	{
		mInitialized = true;

		mName = name;
		mPoolIndex = this - &sCameraPool[0];

		mirrMainNode = mIrrController->addSceneNode();
		if (mirrMainNode == NULL)
		{
			setError(CODE_IRRLICHT_ERROR, "Could not add a scene node to Irrlicht Scene Manager");
			return false;
		}

		mirrCam = mIrrController->addCameraSceneNode();
		if (mirrCam == NULL)
		{
			setError(CODE_IRRLICHT_ERROR, "Could not add a camera scene node to Irrlicht Scene Manager");
			return false;
		}

		mirrLookPoint = mIrrController->addSceneNode();
		if (mirrLookPoint == NULL)
		{
			setError(CODE_IRRLICHT_ERROR, "Could not add a scene node to Irrlicht Scene Manager");
			return false;
		}

		mirrCam->bindTargetAndRotation(true);

		mirrMainNode->addChild(mirrCam);
		mirrMainNode->addChild(mirrLookPoint);
		mirrLookPoint->setPosition(irr::core::vector3df(0.0f, 0.0f, 1.0f));

		mTransform = th;

		return true;
	}


	/*
	* bool kaleidoscope::Camera::destroy()
	*
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used internally by Camera::Destroy() to reset a pool objects state so it may be reused.
	*/
	bool Camera::destroy()
	{
		mIrrController->removeSceneNode(mirrCam);
		mIrrController->removeSceneNode(mirrLookPoint);
		mIrrController->removeSceneNode(mirrMainNode);

		mInitialized = false;
		mNextInFreeList = NULL;

		return true;
	}


	Camera::~Camera(){}


	/*
	* void kaleidoscope::Camera::SerializeIn(const boost::property_tree::ptree& camInfo)
	*
	* In: ptree : property tree representing the cameras internal state.
	* Out: void :
	*
	* Fill in the cameras state with the values provided by the property tree.
	*
	* <near> number </near>
	* <far> number </far>
	* <fov> number </fov>
	*/
	void Camera::SerializeIn(const boost::property_tree::ptree& camInfo)
	{
		using boost::property_tree::ptree;

		boost::optional<F32> ncp = camInfo.get_optional<F32>("near");
		boost::optional<F32> fcp = camInfo.get_optional<F32>("far");
		boost::optional<F32> fov = camInfo.get_optional<F32>("fov");
		boost::optional<std::string> projtype = camInfo.get_optional<std::string>("projection");

		if (ncp)
		{
			setNearClipPlane(*ncp);
		}

		if (fcp)
		{
			setFarClipPlane(*fcp);
		}

		if (fov)
		{
			setFieldOfView(*fov);
		}

	}


	/*
	* boost::property_tree::ptree* kaleidoscope::Camera::SerializeOut()
	*
	* In: void :
	* Out: ptree* : a pointer to a ptree representing the internal state of the camera.
	*
	* THE USER IS RESPONSIBILE FOR DELETING THE POINTER.
	*/
	boost::property_tree::ptree* Camera::SerializeOut()
	{
		using boost::property_tree::ptree;

		ptree* cI = new ptree();

		cI->add<F32>("near", getNearClipPlane());
		cI->add<F32>("far", getFarClipPlane());
		cI->add<F32>("fov", getFieldOfView());

		return cI;
	}


	/*
	* void kaleidoscope::Camera::printState() const 
	*
	* In:  void :
	* Out: void :
	*
	* Print the internal state of the camera
	* NEEDS TO BE REPLACED
	*	replace with CSV
	*/
	void Camera::printState() const 
	{
		gLogManager.log("		mName = %s", getString(mName));
		gLogManager.log("		mPoolIndex = %u", mPoolIndex);
		gLogManager.log("		mNearClip = %f", getNearClipPlane());
		gLogManager.log("		mFarClip = %f", getFarClipPlane());
		gLogManager.log("		mFOV = %f", getFieldOfView());
		
		if (mTransform.valid())
		{
			//
		}
	}



	/*
	* bool kaleidoscope::Camera::StartUp(kaleidoscope::IrrlichtController* irrControl, const boost::property_tree::ptree& properties)
	*
	* In: IrrlichtController* : The irrlicht controller to use to allocate new cameras.
	* In: ptree : The properties used to start up the camera subsystem
	* Out: bool : true on success.
	*			  false on failure.
	*
	* objects = U32 The maximum number of cameras that can be allocated.
	*/
	bool Camera::StartUp(kaleidoscope::IrrlichtController* irrControl, const boost::property_tree::ptree& properties)
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

			sCameraPool = new Camera[MAXNUMOBJECTS];

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

			sFirstFree = &sCameraPool[0];

			for (U32 i = 0; i < MAXNUMOBJECTS - 1; ++i)
			{
				sCameraPool[i].mNextInFreeList = &sCameraPool[i + 1];
			}

			sCameraPool[MAXNUMOBJECTS - 1].mNextInFreeList = NULL;

			return true;
		}
		
		return true;
	}


	/*
	* bool kaleidoscope::Camera::ShutDown()
	*
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used to shutdown the camera subsystem.
	*/
	bool Camera::ShutDown()
	{
		if (initialized)
		{
			initialized = false;

			delete[] sCameraPool;

			Semaphore::Semaphore_destroy(&createSem);
			Semaphore::Semaphore_destroy(&nameSem);
			Semaphore::Semaphore_destroy(&errorSem);

			return true;
		}

		return true;
	}


	/*
	* kaleidoscope::CameraHandle kaleidoscope::Camera::Create(const kaleidoscope::TransformHandle& th)
	*
	* In: TransformHandle : The transform to associate with the new renderable.
	* Out: CameraHandle : An freshly initialized camera.
	*					  CameraHandle::null if the initilization failed.
	*
	* Error handling functions can be used to get the reason the creation failed.
	*/
	CameraHandle Camera::Create(const TransformHandle& th)
	{
		Semaphore::Semaphore_wait(&createSem);

		if (sFirstFree != NULL)
		{
			Camera* const newCam = sFirstFree;
			sFirstFree = newCam->mNextInFreeList;
			bool b = newCam->init(GenerateName(), th);

			// Handle init failure.
			if (b == false)
			{
				newCam->destroy();

				U32 i = (newCam - &sCameraPool[0]);
				sCameraPool[i].mNextInFreeList = sFirstFree;
				sFirstFree = &sCameraPool[i];

				Semaphore::Semaphore_post(&createSem);
				return CameraHandle::null;
			}

			CameraHandle ch(newCam);
			Semaphore::Semaphore_post(&createSem);
			return ch;
		}
		else
		{
			setError(CODE_POOL_FULL, "The maximum number of cameras has been allocated, increase the number of cameras specified in properties at StartUp");
			Semaphore::Semaphore_post(&createSem);
			return CameraHandle::null;
		}
	}


	/*
	* kaleidoscope::CameraHandle kaleidoscope::Camera::Create(const kaleidoscope::TransformHandle& th, const boost::property_tree::ptree& camInfo)
	*
	* In: TransformHandle : The transform to associate with the new renderable.
	* In: ptree : The properties to initialize the camera with.
	* Out: CameraHandle : An initialized camera.
	*					  CameraHandle::null if the initilization failed.
	*
	* Error handling functions can be used to get the reason the creation failed.
	*/
	CameraHandle Camera::Create(const TransformHandle& th, const boost::property_tree::ptree& camInfo)
	{
		CameraHandle ch = Camera::Create(th);
		if (ch != CameraHandle::null)
		{
			ch.getObject()->SerializeIn(camInfo);
		}
		return ch;
	}

	
	/*
	* void kaleidoscope::Camera::Destroy(const kaleidoscope::CameraHandle& ch)
	*
	* In: CameraHandle : The camera to destroy.
	* Out: void :
	*/
	void Camera::Destroy(const CameraHandle& ch)
	{
		Semaphore::Semaphore_wait(&createSem);

		Camera* const c = ch.getObject();

		if (ch.valid())
		{
			U32 i = (c - &sCameraPool[0]);
			sCameraPool[i].destroy();
			sCameraPool[i].mNextInFreeList = sFirstFree;
			sFirstFree = &sCameraPool[i];
		}

		Semaphore::Semaphore_post(&createSem);
	}


	/*
	* void kaleidoscope::Camera::UpdateAll()
	*
	* In: void :
	* Out: void :
	*
	* Synch the underlying Irrlicht State with the cameras state for rendering.
	* Internal use by RenderManager.
	*/
	void Camera::UpdateAll()
	{
		for (U32 i = 0; i < MAXNUMOBJECTS; ++i)
		{
			if (sCameraPool[i].mInitialized)
			{
				Camera* c = &(sCameraPool[i]);

				// Update Position.
				math::vec3 pos = c->transform().getWorldPosition();
				c->mirrMainNode->setPosition(irr::core::vector3df(pos.x, pos.y, pos.z));
				c->mirrMainNode->updateAbsolutePosition();

				// Update Rotation.
				math::quat wori = c->transform().getWorldOrientation();
				math::vec3 weuler = kmath::eulerAnglesNoGimalProtect(wori);
				irr::core::vector3df rot(weuler.x, weuler.y, weuler.z);
				rot = rot * (180.0f / kmath::PI);
				c->mirrMainNode->setRotation(rot);

				// Update all nodes position.
				c->mirrMainNode->updateAbsolutePosition();
				c->mirrCam->updateAbsolutePosition();
				c->mirrLookPoint->updateAbsolutePosition();


				c->mirrCam->setTarget(c->mirrLookPoint->getAbsolutePosition());


			}
		}
	}


	/*
	* bool kaleidoscope::Camera::hasPendingError()
	*
	* In: void :
	* Out: bool : true if there is an error.
	*			  false if there isnt an error.
	*
	* If Create throws an error the system logs an error. The error state stays the same until it is either cleared by
	*	the user or Create throws another error. In this case the previous error is overwritten.
	*/
	bool Camera::hasPendingError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		bool b = mErrorManager.hasPendingError();
		Semaphore::Semaphore_post(&errorSem);
		return b;
	}


	/*
	* void kaleidoscope::Camera::clearError()
	*
	* In: void :
	* Out: void :
	*
	* If there was an error in the transform system clear it out.
	* Used after processing an error to tell when the next error occurs.
	*/
	void Camera::clearError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.clearError();
		Semaphore::Semaphore_post(&errorSem);
	}


	/*
	* kaleidoscope::ErrorCode kaleidoscope::Camera::getErrorCode()
	*
	* In: void :
	* Out: ErrorCode : The error code representing the current state of the transform system.
	*/
	ErrorCode Camera::getErrorCode()
	{ 
		Semaphore::Semaphore_wait(&errorSem);
		ErrorCode ec = mErrorManager.getErrorCode();
		Semaphore::Semaphore_post(&errorSem);
		return ec;
	}


	/*
	* std::string kaleidoscope::Camera::getErrorString()
	*
	* In: void :
	* Out: ErrorCode : The detailed string explaining why the error happened.
	*/
	std::string Camera::getErrorString()
	{
		Semaphore::Semaphore_wait(&errorSem);
		std::string es = mErrorManager.getErrorString();
		Semaphore::Semaphore_post(&errorSem);
		return es;
	}


	/*
	* void kaleidoscope::Camera::setError(kaleidoscope::ErrorCode ec, const std::string& es)
	*
	* In: ErrorCode : What error happened in the system.
	* In: string : The detailed description of what happened.
	* Out: void :
	*
	* used internally.
	*/
	void Camera::setError(ErrorCode ec, const std::string& es)
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.setError(ec, es); 
		Semaphore::Semaphore_post(&errorSem);
	}


	/*
	* kaleidoscope::StringID kaleidoscope::Camera::GenerateName()
	*
	* In: void :
	* Out: StringID : The name generated for the next camera.
	*
	* Used internally to make sure each camera has a unique name.
	*/
	StringID Camera::GenerateName()
	{
		char buff[25];
		Semaphore::Semaphore_wait(&nameSem);
		I32 n = sprintf_s(buff, "Camera__%u", numCams++);
		Semaphore::Semaphore_post(&nameSem);
		StringID name = internString(buff);
		return name;
	}

	const char * Camera::LUA_TYPE_NAME = "kaleidoscope.camera";
	StringID Camera::NAME = hashCRC32("camera");
	U32 Camera::MAXNUMOBJECTS = 0;

	bool Camera::initialized = false;

	const U32 Camera::DEFAULTMAX = 10;
	const F32 Camera::DEFAULTNEARCLIP = 0.3f;
	const F32 Camera::DEFAULTFARCLIP = 10.0f;
	const F32 Camera::DEFAULTFOV = 60.0f;

	U32 Camera::numCams = 0;
	Camera* Camera::sFirstFree = NULL;
	Camera* Camera::sCameraPool = NULL;

	kaleidoscope::IrrlichtController* Camera::mIrrController = NULL;

	ErrorManager Camera::mErrorManager;
}