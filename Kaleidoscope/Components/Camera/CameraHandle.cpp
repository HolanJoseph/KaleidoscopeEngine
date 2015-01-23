#include <Components/Camera/CameraHandle.h>
#include <Components/Camera/Camera.h>

namespace kaleidoscope
{

	//////////////////////////////////////////////////////////////////////////
	/// Handle Functions.
	//////////////////////////////////////////////////////////////////////////

	const CameraHandle CameraHandle::null = CameraHandle();

	bool CameraHandle::valid() const { return (getObject() == NULL ? false : true); }

	CameraHandle::CameraHandle(const Camera* c) : mName(c->mName), mPoolIndex(c->mPoolIndex) {}

	Camera* CameraHandle::getObject() const
	{
		Camera* pC = &Camera::sCameraPool[mPoolIndex];

		if ((pC != NULL) && (pC->mInitialized) && (pC->mName == mName))
		{
			return pC;
		}
		else
		{
			return NULL;
		}
	}




	//////////////////////////////////////////////////////////////////////////
	/// Camera Interface.
	//////////////////////////////////////////////////////////////////////////

	void CameraHandle::SerializeIn(const boost::property_tree::ptree& camInfo) { getObject()->SerializeIn(camInfo); }
	boost::property_tree::ptree* CameraHandle::SerializeOut() { return getObject()->SerializeOut(); }

	StringID CameraHandle::getName() const { return getObject()->getName(); }
	TransformHandle CameraHandle::transform() { return getObject()->transform(); }

	F32  CameraHandle::getNearClipPlane() const { return getObject()->getNearClipPlane(); }
	void CameraHandle::setNearClipPlane(F32 ncp) { getObject()->setNearClipPlane(ncp); }

	F32  CameraHandle::getFarClipPlane() const { return getObject()->getFarClipPlane(); }
	void CameraHandle::setFarClipPlane(F32 fcp) { getObject()->setFarClipPlane(fcp); }

	F32  CameraHandle::getFieldOfView() const { return getObject()->getFieldOfView(); }
	void CameraHandle::setFieldOfView(F32 fov) { getObject()->setFieldOfView(fov); }

	void CameraHandle::printState() const { getObject()->printState(); }




	//////////////////////////////////////////////////////////////////////////
	/// Static Camera Interface.
	//////////////////////////////////////////////////////////////////////////

	StringID CameraHandle::NAME = Camera::NAME;
	U32 CameraHandle::MAXNUMOBJECTS = 0;
	const char * CameraHandle::LUA_TYPE_NAME = Camera::LUA_TYPE_NAME;

	bool CameraHandle::StartUp(kaleidoscope::IrrlichtController* irrControl, const boost::property_tree::ptree& properties)
	{
		bool b = Camera::StartUp(irrControl, properties);
		NAME = Camera::NAME;
		MAXNUMOBJECTS = Camera::MAXNUMOBJECTS;
		LUA_TYPE_NAME = Camera::LUA_TYPE_NAME;
		return b;
	}

	bool CameraHandle::ShutDown() { return Camera::ShutDown(); }

	CameraHandle CameraHandle::Create(const TransformHandle& th) { return Camera::Create(th); }
	CameraHandle CameraHandle::Create(const TransformHandle& th, const boost::property_tree::ptree& camInfo) { return Camera::Create(th, camInfo); }

	void CameraHandle::Destroy(const CameraHandle& ch) { Camera::Destroy(ch); }

	void CameraHandle::UpdateAll() { Camera::UpdateAll(); }

	bool CameraHandle::hasPendingError() { return Camera::hasPendingError(); }
	void CameraHandle::clearError() { Camera::clearError(); }
	ErrorCode CameraHandle::getErrorCode() { return Camera::getErrorCode(); }
	std::string CameraHandle::getErrorString() { return Camera::getErrorString(); }
}