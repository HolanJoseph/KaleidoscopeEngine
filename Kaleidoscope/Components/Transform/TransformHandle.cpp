#include <Components/Transform/TransformHandle.h>
#include <Components/Transform/Transform.h>

namespace kaleidoscope
{

	//////////////////////////////////////////////////////////////////////////
	/// Handle Functions.
	//////////////////////////////////////////////////////////////////////////

	const TransformHandle TransformHandle::null = TransformHandle();

	bool TransformHandle::valid() const { return (getObject() == NULL ? false : true); }

	TransformHandle::TransformHandle(const Transform* t) : mName(t->mName), mPoolIndex(t->mPoolIndex){}


	Transform* TransformHandle::getObject() const
	{
		Transform* pT = &Transform::TransformPool[mPoolIndex];

		if ((pT != NULL) && (pT->mInitialized) && (pT->mName == mName))
		{
			return pT;
		}
		else
		{
			return NULL;
		}
	}





	//////////////////////////////////////////////////////////////////////////
	/// Transform Interface.
	//////////////////////////////////////////////////////////////////////////

	void TransformHandle::SerializeIn(const boost::property_tree::ptree& transformInfo) { getObject()->SerializeIn(transformInfo); }
	boost::property_tree::ptree* TransformHandle::SerializeOut() { return getObject()->SerializeOut(); }

	StringID TransformHandle::getName() const { return getObject()->getName(); }

	TransformHandle TransformHandle::getParent() const      { return getObject()->getParent(); }
	void TransformHandle::setParent(const TransformHandle& parent) { getObject()->setParent(parent); }

	TransformHandle TransformHandle::getChild(StringID name)   { return getObject()->getChild(name); }
	std::list<TransformHandle>* TransformHandle::getChildren() { return getObject()->getChildren(); }
	void TransformHandle::addChild(const TransformHandle& child)      { getObject()->addChild(child); }
	void TransformHandle::removeChild(const TransformHandle& child)   { getObject()->removeChild(child); }

	math::vec3 TransformHandle::getLocalPosition() const          { return getObject()->getLocalPosition(); }
	void TransformHandle::setLocalPosition(const math::vec3& newPos)     { getObject()->setLocalPosition(newPos); }
	void TransformHandle::translate(const math::vec3& translationVector) { getObject()->translate(translationVector); }

	math::vec3 TransformHandle::getLocalScale() const { return getObject()->getLocalScale();}
	F32 TransformHandle::getLocalXScale() const       { return getObject()->getLocalXScale(); }
	F32 TransformHandle::getLocalYScale() const       { return getObject()->getLocalYScale(); }
	F32 TransformHandle::getLocalZScale() const       { return getObject()->getLocalZScale(); }

	void TransformHandle::setLocalScale(const math::vec3& newScale) { getObject()->setLocalScale(newScale); }
	void TransformHandle::setLocalXScale(F32 newXScale)      { getObject()->setLocalXScale(newXScale); }
	void TransformHandle::setLocalYScale(F32 newYScale)      { getObject()->setLocalYScale(newYScale); }
	void TransformHandle::setLocalZScale(F32 newZScale)      { getObject()->setLocalZScale(newZScale); }

	math::quat TransformHandle::getLocalOrientation() const                 { return getObject()->getLocalOrientation(); }
	void TransformHandle::setLocalOrientation(const math::quat& newOrientation)    { getObject()->setLocalOrientation(newOrientation); }
	void TransformHandle::rotateAroundAxisLocal(const math::vec3& axis, F32 theta) { getObject()->rotateAroundAxisLocal(axis, theta); }
	void TransformHandle::rotateAroundAxisWorld(const math::vec3& axis, F32 theta) { getObject()->rotateAroundAxisWorld(axis, theta); }

	math::quat TransformHandle::getWorldOrientation() const { return getObject()->getWorldOrientation(); }
	math::vec3 TransformHandle::getWorldScale() const { return getObject()->getWorldScale(); }
	math::vec3 TransformHandle::getWorldPosition() const { return getObject()->getWorldPosition(); }
	void TransformHandle::setWorldPosition(const math::vec3& newPosition) { getObject()->setWorldPosition(newPosition); }

	void TransformHandle::rotateAroundWorldPoint(const math::vec3& pt, const math::vec3& axis, F32 theta) { getObject()->rotateAroundWorldPoint(pt, axis, theta); }
	void TransformHandle::lookAt(const math::vec3& pt, const math::vec3& upHint) { getObject()->lookAt(pt, upHint); }

	math::mat4 TransformHandle::getModelToParentMatrix() const { return getObject()->getModelToParentMatrix(); };
	math::mat4 TransformHandle::getLocalToWorldMatrix() const { return getObject()->getLocalToWorldMatrix(); }
	math::mat4 TransformHandle::getWorldToLocalMatrix() const { return getObject()->getWorldToLocalMatrix(); }
	
	void TransformHandle::printState() const { getObject()->printState(); }




	//////////////////////////////////////////////////////////////////////////
	/// Static Renderable Interface.
	//////////////////////////////////////////////////////////////////////////

	StringID TransformHandle::NAME = Transform::NAME;
	U32 TransformHandle::MAXNUMOBJECTS = 0;

	bool TransformHandle::StartUp(const boost::property_tree::ptree& properties)
	{ 
		bool b = Transform::StartUp(properties);
		NAME = Transform::NAME;
		MAXNUMOBJECTS = Transform::MAXNUMOBJECTS;
		return b;
	}

	bool TransformHandle::ShutDown() { return Transform::ShutDown(); }

	TransformHandle TransformHandle::Create() { return Transform::Create(); }
	TransformHandle TransformHandle::Create(const boost::property_tree::ptree& transformInfo) { return Transform::Create(transformInfo); }

	void TransformHandle::Destroy(const TransformHandle& transformToDestroy) { Transform::Destroy(transformToDestroy); }

	bool TransformHandle::hasPendingError() { return Transform::hasPendingError(); }
	void TransformHandle::clearError() { Transform::clearError(); }
	ErrorCode TransformHandle::getErrorCode() { return Transform::getErrorCode(); }
	std::string TransformHandle::getErrorString() { return Transform::getErrorString(); }
}
