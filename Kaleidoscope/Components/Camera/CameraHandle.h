#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Debug/ErrorManagement/ErrorCodes.h>

#include <Rendering/Irrlicht/IrrlichtController.h>

#include <Components/Transform/TransformHandle.h>

namespace kaleidoscope
{
	class Camera;

	class CameraHandle
	{
		friend class Camera;
		

		//////////////////////////////////////////////////////////////////////////
		/// Handle Functions.
		//////////////////////////////////////////////////////////////////////////

		// Comparison Operations.
		friend inline bool operator==(const CameraHandle& lhs, const CameraHandle& rhs);
		friend inline bool operator!=(const CameraHandle& lhs, const CameraHandle& rhs);

		// Copy Control.
	public:
		CameraHandle(const CameraHandle& c) : mName(c.mName), mPoolIndex(c.mPoolIndex) {};
		inline CameraHandle& operator=(const CameraHandle& rhs);

		static const CameraHandle null;

		CameraHandle() : mName(NULLNAME), mPoolIndex(0) {};
		~CameraHandle(){};

		// Conversions.
		CameraHandle(const Camera* c);
		Camera* getObject() const;

		bool valid() const;




		//////////////////////////////////////////////////////////////////////////
		/// Camera Interface.
		//////////////////////////////////////////////////////////////////////////

		void SerializeIn(const boost::property_tree::ptree& camInfo);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const;

		TransformHandle transform();

		F32 getNearClipPlane() const;
		void setNearClipPlane(F32 ncp);

		F32 getFarClipPlane() const;
		void setFarClipPlane(F32 fcp);

		F32 getFieldOfView() const;
		void setFieldOfView(F32 fov);

		void printState() const;




		//////////////////////////////////////////////////////////////////////////
		/// Static Camera Interface.
		//////////////////////////////////////////////////////////////////////////

		static StringID NAME;
		static U32 MAXNUMOBJECTS;
		static const char * LUA_TYPE_NAME;

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




		//////////////////////////////////////////////////////////////////////////
		/// Handle Data.
		//////////////////////////////////////////////////////////////////////////

	private:
		StringID mName;
		U32 mPoolIndex;

		static const StringID NULLNAME = 0;

	};

	CameraHandle& CameraHandle::operator=(const CameraHandle& rhs)
	{
		mName = rhs.mName;
		mPoolIndex = rhs.mPoolIndex;
		return *this;
	};

	bool operator==(const CameraHandle& lhs, const CameraHandle& rhs)
	{
		if ((lhs.mName == rhs.mName) && (lhs.mPoolIndex == rhs.mPoolIndex))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool operator!=(const CameraHandle& lhs, const CameraHandle& rhs)
	{
		return !(lhs == rhs);
	}
}