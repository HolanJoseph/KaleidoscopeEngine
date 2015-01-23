#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Debug/ErrorManagement/ErrorCodes.h>
#include <Debug/ErrorManagement/ErrorManager.h>

#include <Components/Transform/TransformHandle.h>

#include <Rendering/Irrlicht/IrrlichtController.h>
#include <irrlicht.h>

#include <boost/property_tree/ptree.hpp>

namespace kaleidoscope
{
	class CameraHandle;

	class Camera
	{
		friend class CameraHandle;


		static StringID NAME;
		static U32 MAXNUMOBJECTS;
		static const char * LUA_TYPE_NAME;


		bool mInitialized;
		union
		{
			class
			{
			public:
				StringID mName;
				U32 mPoolIndex;

				irr::scene::ISceneNode* mirrMainNode;
				irr::scene::ICameraSceneNode* mirrCam;
				irr::scene::ISceneNode* mirrLookPoint;

				TransformHandle mTransform;
			};
			Camera* mNextInFreeList;
		};


		Camera();
		bool init(const StringID name, TransformHandle th);
		bool destroy();
		~Camera();

		void SerializeIn(const boost::property_tree::ptree& camInfo);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const { return mName; };
		TransformHandle transform() { return mTransform; };

		F32 getNearClipPlane() const { return mirrCam->getNearValue(); };
		void setNearClipPlane(F32 ncp) { mirrCam->setNearValue(ncp); };

		F32 getFarClipPlane() const { return mirrCam->getFarValue(); };
		void setFarClipPlane(F32 fcp) { mirrCam->setFarValue(fcp); };

		F32 getFieldOfView() const { return mirrCam->getFOV(); };
		void setFieldOfView(F32 fov) { mirrCam->setFOV(fov); };

		void printState() const;
		



		static bool StartUp(kaleidoscope::IrrlichtController* irrControl, const boost::property_tree::ptree& properties);
		static bool ShutDown();

		static CameraHandle Create(const TransformHandle& th);
		static CameraHandle Create(const TransformHandle& th, const boost::property_tree::ptree& camInfo);

		static void Destroy(const CameraHandle& ch);

		static void UpdateAll();

		static bool hasPendingError();
		static void clearError();
		static ErrorCode getErrorCode();
		static std::string getErrorString();
		static void setError(ErrorCode ec, const std::string& es);



		static bool initialized;

		static const U32 DEFAULTMAX;
		static const F32 DEFAULTNEARCLIP;
		static const F32 DEFAULTFARCLIP;
		static const F32 DEFAULTFOV;

		static U32 numCams;
		static StringID GenerateName();

		static Camera* sFirstFree;
		static Camera* sCameraPool;

		static IrrlichtController* mIrrController;

		static ErrorManager mErrorManager;
	};
}