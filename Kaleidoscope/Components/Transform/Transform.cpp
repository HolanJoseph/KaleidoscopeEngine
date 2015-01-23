
#include <Components/Transform/Transform.h>

#include <Debug/Logging/SDLLogManager.h>

#include <Synchronization/Locks/Mutex/Mutex.h>
#include <Synchronization/Locks/Semaphore/Semaphore.h>
#include <Synchronization/Threads/Thread.h>

#include <algorithm>
#include <cstdio>

#include <string>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <Utility/Parsing/Lowerize.h>
#include <Utility/Parsing/parseMathsFromStrings.h>
#include <Utility/Parsing/generateStringFromMaths.h>


extern kaleidoscope::SDLLogManager gLogManager;

namespace kaleidoscope
{
	static Semaphore createSem;
	static Semaphore nameSem;
	static Mutex childLock;
	static Semaphore errorSem;

	Transform::Transform()
	{
		mInitialized = false;
		mNextInFreeList = NULL;
	}

	/*
	* bool kaleidoscope::Transform::init(const StringID name)
	*
	* In: StringID name : The name to initialize the transform with.
	* Out: bool : Always returns true.
	*
	* Used internally by Transform::Create() to initialize a pool object.
	*/
	bool Transform::init(const StringID name)
	{
		mInitialized = true;

		mName = name;
		mPoolIndex = this - &TransformPool[0];

		mParentTransform = TransformHandle::null;
		mChildTransforms = std::list<TransformHandle>();

		mPosition = DEFAULTPOSITION;
		mOrientation = DEFAULTORIENTATION;
		mScale = DEFAULTSCALE;

		return true;
	}


	/*
	* bool kaleidoscope::Transform::destroy()
	*
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used internally by Transform::Destroy() to reset a pool objects state so it may be reused.
	*/
	bool Transform::destroy()
	{
		setParent(TransformHandle::null);

		std::list<TransformHandle>* tmplist = getChildren();
		for (std::list<TransformHandle>::iterator child = tmplist->begin(); child != tmplist->end(); ++child)
		{
			removeChild(*child);
		}
		delete tmplist;

		mInitialized = false;
		mNextInFreeList = NULL;

		return true;
	}


	Transform::~Transform(){}


	/*
	* void kaleidoscope::Transform::SerializeIn(const boost::property_tree::ptree& transformInfo)
	* 
	* In: ptree transformInfo : property tree representing the transforms internal state.
	* Out: void :
	* 
	* Fill in the transforms state with the values provided by the property tree.
	*
	* <position>(x, y, z)</position>
	* <orientation>(w, x, y, z)</orientation>
	* <scale>(x, y, z)</scale>
	*/
	void Transform::SerializeIn(const boost::property_tree::ptree& transformInfo)
	{
		using boost::property_tree::ptree;

		boost::optional<std::string> pos = transformInfo.get_optional<std::string>("position");
		boost::optional<std::string> ori = transformInfo.get_optional<std::string>("orientation");
		boost::optional<std::string> scale = transformInfo.get_optional<std::string>("scale");

		bool b = false;

		if (pos)
		{
			mPosition = stringToVec3(pos->c_str(), b);
		}

		if (ori)
		{
			mOrientation = stringToQuat(ori->c_str(), b);
		}

		if (scale)
		{
			mScale = stringToVec3(scale->c_str(), b);
		}

	}


	/*
	* boost::property_tree::ptree* kaleidoscope::Transform::SerializeOut()
	*
	* In: void :
	* Out: ptree* : a pointer to a ptree representing the internal state of the transform.
	*
	* THE USER IS RESPONSIBILE FOR DELETING THE POINTER.
	*/
	boost::property_tree::ptree* Transform::SerializeOut()
	{
		using boost::property_tree::ptree;

		ptree* tI = new ptree();

		std::string pos = vec3ToString(mPosition);
		tI->add("position", pos);

		std::string ori = quatToString(mOrientation);
		tI->add("orientation", ori);

		std::string scale = vec3ToString(mScale);
		tI->add("scale", scale);

		return tI;
	}


	/*
	* kaleidoscope::TransformHandle kaleidoscope::Transform::getParent() const
	*
	* In: void : 
	* Out: TransformHandle : This transforms parent transform. TransformHandle::null if it has no parent.
	*/
	TransformHandle Transform::getParent() const
	{
		return mParentTransform;
	}


	/*
	* void kaleidoscope::Transform::setParent(const TransformHandle& parent)
	*
	* In: TransformHandle parent: The new parent for this transform.
	* Out: void :
	*
	* If TransformHandle::null is provided as the parent the transforms parent is removed.
	*/
	void Transform::setParent(const TransformHandle& parent)
	{
		if (parent.getObject() != NULL)
		{
			parent.getObject()->addChild(TransformHandle(this));
		}
		else if (mParentTransform != TransformHandle::null)
		{
			mParentTransform.getObject()->removeChild(TransformHandle(this));
		}
	}


	/*
	* kaleidoscope::TransformHandle kaleidoscope::Transform::getChild(StringID name)
	*
	* In: StringID name: The name of the child to get.
	* Out: TransformHandle : the transform of the child. Or null if the child doesn't exist.
	*/
	TransformHandle Transform::getChild(StringID name)
	{
		TransformHandle child;

		for (std::list<TransformHandle>::iterator curr = mChildTransforms.begin(); curr != mChildTransforms.end(); ++curr)
		{
			if ((*curr).getObject()->getName() == name)
			{
				child = *curr;
				break;
			}
		}
		if (child != *(mChildTransforms.end()))
		{
			return child;
		}

		return TransformHandle::null;
	}

	/*
	* std::list<kaleidoscope::TransformHandle>* kaleidoscope::Transform::getChildren()
	*
	* In: void :
	* Out: list<TransformHandle>* : A pointer to a list of the transforms children.
	*
	* IT IS THE USERS RESPONSIBILITY TO DELETE THE POINTER.
	*/
	std::list<TransformHandle>* Transform::getChildren()
	{
		std::list<TransformHandle>* mcp = new std::list<TransformHandle>(mChildTransforms.cbegin(), mChildTransforms.cend());
		return mcp;
	}

	/*
	* void kaleidoscope::Transform::addChild(const kaleidoscope::TransformHandle& child)
	*
	* In: TransformHandle child : The child to add as a child.
	* Out: void :
	*/
	void Transform::addChild(const TransformHandle& child)
	{
		if (child.valid())
		{
			Mutex::Mutex_lock(&childLock);
			mChildTransforms.push_back(child);
			if (child.getParent().valid())
			{
				child.getParent().removeChild(child);
			}
			child.getObject()->mParentTransform = TransformHandle(this);
			Mutex::Mutex_unlock(&childLock);
		}
	}

	/*
	* void kaleidoscope::Transform::removeChild(const kaleidoscope::TransformHandle& child)
	*
	* In: TransformHandle child : The child to remove.
	* Out: void :
	*/
	void Transform::removeChild(const TransformHandle& child)
	{
		Mutex::Mutex_lock(&childLock);
		const std::list<TransformHandle>::iterator childLocation = std::find(mChildTransforms.begin(), mChildTransforms.end(), child);
		if (childLocation != mChildTransforms.end() && (*childLocation).valid())
		{
			mChildTransforms.erase(childLocation);
			child.getObject()->mParentTransform = TransformHandle::null;
		}
		Mutex::Mutex_unlock(&childLock);
	}



	/*
	* math::vec3 kaleidoscope::Transform::getLocalPosition() const
	*
	* In: void :
	* Out: vec3 : The transforms position relative to its parents origin.
	*/
	math::vec3 Transform::getLocalPosition() const
	{
		return mPosition;
	}


	/*
	* void kaleidoscope::Transform::setLocalPosition(const math::vec3& newPos)
	*
	* In: vec3 : The transforms new position relative to its parents origin.
	* Out: void :
	*/
	void Transform::setLocalPosition(const math::vec3& newPos)
	{
		mPosition = newPos;
	}


	/*
	* void kaleidoscope::Transform::translate(const math::vec3& translationVector)
	*
	* In: vec3 : How far to move the transform.
	* Out:
	*
	* TRANSLATION IS APPLIED IN LOCAL SPACE.
	*/
	void Transform::translate(const math::vec3& translationVector)
	{
		math::vec4 tv = getModelToParentMatrix() * math::vec4(translationVector.x, translationVector.y, translationVector.z, 0);
		math::vec3 tv3(tv.x, tv.y, tv.z);
		mPosition += tv3;
	}



	/*
	* math::vec3 kaleidoscope::Transform::getLocalScale() const
	*
	* In: void :
	* Out: vec3 : The transforms scale relative to its parents scale.
	*/
	math::vec3 Transform::getLocalScale() const
	{
		return mScale;
	}


	/*
	* F32 kaleidoscope::Transform::getLocalXScale() const
	*
	* In: void :
	* Out: F32 : The transforms x scale relative to its parents scale.
	*/
	F32 Transform::getLocalXScale() const
	{
		return mScale.x;
	}


	/*
	* F32 kaleidoscope::Transform::getLocalYScale() const
	*
	* In: void :
	* Out: F32 : The transforms y scale relative to its parents scale.
	*/
	F32 Transform::getLocalYScale() const
	{
		return mScale.y;
	}


	/*
	* F32 kaleidoscope::Transform::getLocalZScale() const
	*
	* In: void :
	* Out: F32 : The transforms z scale relative to its parents scale.
	*/
	F32 Transform::getLocalZScale() const
	{
		return mScale.z;
	}


	/*
	* void kaleidoscope::Transform::setLocalScale(const math::vec3& newScale)
	*
	* In: vec3 : The new scales for the transform relative to its parents scale.
	* Out: void :
	*/
	void Transform::setLocalScale(const math::vec3& newScale)
	{
		mScale = newScale;
	}


	/*
	* void kaleidoscope::Transform::setLocalXScale(F32 newXScale)
	*
	* In: F32 : The new x scale for the transform relative to its parents scale.
	* Out: void :
	*/
	void Transform::setLocalXScale(F32 newXScale)
	{
		mScale.x = newXScale;
	}


	/*
	* void kaleidoscope::Transform::setLocalYScale(F32 newYScale)
	*
	* In: F32 : The new y scale for the transform relative to its parents scale.
	* Out: void :
	*/
	void Transform::setLocalYScale(F32 newYScale)
	{
		mScale.y = newYScale;
	}


	/*
	* void kaleidoscope::Transform::setLocalZScale(F32 newZScale)
	*
	* In: F32 : The new z scale for the transform relative to its parents scale.
	* Out: void :
	*/
	void Transform::setLocalZScale(F32 newZScale)
	{
		mScale.z = newZScale;
	}



	/*
	* math::quat kaleidoscope::Transform::getLocalOrientation() const
	*
	* In: void :
	* Out: quat : This objects orientation relative to its parents orientation.
	*/
	math::quat Transform::getLocalOrientation() const
	{
		return mOrientation;
	}


	/*
	* void kaleidoscope::Transform::setLocalOrientation(const math::quat& newOrientation)
	*
	* In: quat : The new orientation for this transform relative to its parents orientation.
	* Out: void :
	*/
	void Transform::setLocalOrientation(const math::quat& newOrientation)
	{
		mOrientation = newOrientation;
	}


	/*
	* void kaleidoscope::Transform::rotateAroundAxisWorld(const math::vec3& axis, F32 theta)
	*
	* In: vec3 : The axis, in world frame, to rotate around. 
	* In: F32 : The angle to rotate in radians.
	* Out: void :
	*/
	void Transform::rotateAroundAxisWorld(const math::vec3& axis, F32 theta)
	{
		using math::quat;

		math::mat4 MP = (getParent().valid() ? getParent().getWorldToLocalMatrix() : math::mat4());
		math::vec4 aP = MP * math::vec4(axis.x, axis.y, axis.z, 0.0f);
		math::vec3 a(aP.x, aP.y, aP.z);

		quat p = kmath::rotationAboutAxisQuat(a, theta);
		mOrientation = p * mOrientation;
	}


	/*
	* void kaleidoscope::Transform::rotateAroundAxisLocal(const math::vec3& axis, F32 theta)
	*
	* In: vec3 : The axis, in local frame, to rotate around.
	* In: F32 : The angle to rotate in radians.
	* Out: void :
	*/
	void Transform::rotateAroundAxisLocal(const math::vec3& axis, F32 theta)
	{
		math::vec4 a = getModelToParentMatrix() * math::vec4(axis.x, axis.y, axis.z, 0.0f);
		a = math::normalize(a);

		math::quat p = kmath::rotationAboutAxisQuat(math::vec3(a.x, a.y, a.z), theta);
		mOrientation = p * mOrientation;
	}



	/*
	* void kaleidoscope::Transform::rotateAroundWorldPoint(const math::vec3& pt, const math::vec3& axis, F32 theta)
	*
	* In: vec3 : The point to rotate the transform around, specified in world frame.
	* In: vec3 : The axis to rotate around, specified in world frame.
	* In: F32 : The angle to rotate in radians.
	* Out: void :
	*/
	void Transform::rotateAroundWorldPoint(const math::vec3& pt, const math::vec3& axis, F32 theta)
	{
		math::mat4 MP = (getParent().valid() ? getParent().getWorldToLocalMatrix() : math::mat4()); // Matrix to transform from world frame to parent frame.
		
		math::vec4 ptP = MP * math::vec4(pt.x, pt.y, pt.z, 1.0f); // The provided point in the parent frame.
		math::vec4 aP = MP * math::vec4(axis.x, axis.y, axis.z, 0.0f); // The provided axis in the parent frame.
		aP = math::normalize(aP);

		math::vec3 mPosP = mPosition - math::vec3(ptP.x, ptP.y, ptP.z); // the transforms position translated by -pt in the parent frame.
		math::quat v(0.0f, mPosP.x, mPosP.y, mPosP.z); // quaternion representation of our position.

		math::quat q = kmath::rotationAboutAxisQuat(math::vec3(aP.x, aP.y, aP.z), theta);
		math::quat qInv(q.w, -q.x, -q.y, -q.z);
		math::quat vPrime = q * v * qInv;

		math::vec3 nPos = math::vec3(vPrime.x, vPrime.y, vPrime.z) + math::vec3(ptP.x, ptP.y, ptP.z);
		mPosition = nPos;

		rotateAroundAxisLocal(axis, theta);
	}

	/*
	* math::quat kaleidoscope::getRotationBetweenTwoVectors(const math::vec3& start, const math::vec3& end)
	*
	* In: vec3 start: The starting vector.
	* In: vec3 end: The vector to rotate start to.
	* Out: quat : A quaternion representing the rotation necessary to align start with end.
	*
	* used by lookat()
	*/
	math::quat getRotationBetweenTwoVectors(const math::vec3& start, const math::vec3& end)
	{
		math::vec3 nStart = math::normalize(start);
		math::vec3 nEnd = math::normalize(end);

		F32 costheta = math::dot(nStart, nEnd);
		math::vec3 a;

		F32 e = .001f;
		if (costheta < (-1.0f + e))
		{
			a = math::cross(math::vec3(0.0f, 0.0f, 1.0f), nStart);
			//F32 l2norm = sqrt();
			if (math::length2(a) < 0.01f)
			{
				a = math::cross(math::vec3(1.0f, 0.0f, 0.0f), nStart);
			}

			

			a = math::normalize(a);
			return kmath::rotationAboutAxisQuat(a, kmath::PI);
		}

		a = math::cross(nStart, nEnd);

		F32 s = sqrt((1 + costheta) * 2);
		F32 invs = 1 / s;
		return math::quat(s * 0.5f, a.x * invs, a.y * invs, a.z * invs);
	}


	/*
	* void kaleidoscope::Transform::lookAt(const math::vec3& pt, const math::vec3& upHint)
	*
	* In: vec3 pt : The point to lookAt.
	* In: vec3 uphint : ------
	* Out: void :
	* 
	* Rotates the transform so that its forward vector is pointing at pt.
	*/
	void Transform::lookAt(const math::vec3& pt, const math::vec3& upHint)
	{
		using math::vec3;
		using math::vec4;
		using math::mat4;
		using math::quat;

		mat4 MP = (getParent().valid() ? getParent().getWorldToLocalMatrix() : math::mat4());
		vec4 ptP4 = MP * vec4(pt.x, pt.y, pt.z, 1.0f);
		vec3 direction = vec3(ptP4.x, ptP4.y, ptP4.z) - mPosition;

		mat4 MtoP = getModelToParentMatrix();
		vec4 from = MtoP * vec4(0.0f, 0.0f, 1.0f, 0.0f);
		vec3 from3 = vec3(from.x, from.y, from.z);

		quat rotationalQuat = getRotationBetweenTwoVectors(from3, direction);
		
		mOrientation = rotationalQuat * mOrientation;
	}



	/*
	* math::quat kaleidoscope::Transform::getWorldOrientation() const
	*
	* In: void :
	* Out: quat : The transforms orientation relative to the world. 
	*/
	math::quat Transform::getWorldOrientation() const
	{
		math::quat q = (getParent().valid() ? getParent().getWorldOrientation() * mOrientation : mOrientation);
		return q;
	}


	/*
	* math::vec3 kaleidoscope::Transform::getWorldScale() const
	*
	* In: void :
	* Out: vec3 : The transforms scale relative to the world.
	*/
	math::vec3 Transform::getWorldScale() const
	{
		math::vec3 s = getLocalScale();
		TransformHandle p = getParent();
		if (p.valid())
		{
			math::vec3 pS = p.getWorldScale();
			s.x *= pS.x;
			s.y *= pS.y;
			s.z *= pS.z;
		}
		return s;
	}


	/*
	* math::vec3 kaleidoscope::Transform::getWorldPosition() const
	*
	* In: void :
	* Out: vec3 : The transforms position relative to the world.
	*/
	math::vec3 Transform::getWorldPosition() const
	{
		math::vec4 p = getLocalToWorldMatrix() * math::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		return math::vec3(p.x, p.y, p.z);
	}


	/*
	* void kaleidoscope::Transform::setWorldPosition(const math::vec3& newPosition)
	*
	* In: vec3 : The new position for the transform specified relative to the worlds origin.
	* Out: void :
	*/
	void Transform::setWorldPosition(const math::vec3& newPosition)
	{
		math::mat4 M = (getParent().valid() ? getParent().getWorldToLocalMatrix() : math::mat4());
		math::vec4 newPosp = M * math::vec4(newPosition.x, newPosition.y, newPosition.z, 1.0f);
		mPosition = math::vec3(newPosp.x, newPosp.y, newPosp.z);
	}


	/*
	* math::mat4 kaleidoscope::Transform::getModelToParentMatrix() const
	*
	* In: void :
	* Out: mat4 : The transformation that takes a point in the objects model space and brings it into its parents space.
	*/
	math::mat4 Transform::getModelToParentMatrix() const
	{
		math::mat4 M = (kmath::translationMat4(mPosition) * math::toMat4(mOrientation) * kmath::scaleMat4(mScale));
		return M;
	}


	/*
	* math::mat4 kaleidoscope::Transform::getLocalToWorldMatrix() const
	*
	* In: void :
	* Out: mat4 : The transformation that takes a point in the objects model space and brings it into world space.
	*/
	math::mat4 Transform::getLocalToWorldMatrix() const
	{
		math::mat4 M;
		if (getParent().valid())
		{
			M = getParent().getLocalToWorldMatrix() * (kmath::translationMat4(mPosition) * math::toMat4(mOrientation) * kmath::scaleMat4(mScale));
		} 
		else
		{
			M = (kmath::translationMat4(mPosition) * math::toMat4(mOrientation) * kmath::scaleMat4(mScale));
		}

		return M;
	}


	/*
	* math::mat4 kaleidoscope::Transform::getWorldToLocalMatrix() const
	*
	* In: void :
	* Out: mat4 : The transformation that takes a point in world space and brings it into this transforms local space.
	*/
	math::mat4 Transform::getWorldToLocalMatrix() const
	{
		math::mat4 M = math::inverse(getLocalToWorldMatrix());

		return M;
	}



	/*
	* void kaleidoscope::Transform::printState() const
	* 
	* Print the internal state of the transform
	* NEEDS TO BE CHANGED
	*
	* Current idea is CSV string return
	*/
	void Transform::printState() const
	{
		gLogManager.log("		mName = %s", getString(mName));
		gLogManager.log("		mPoolIndex = %i", mPoolIndex);
		(getParent().valid() ? gLogManager.log("		mParentTransform = %s", getString(getParent().getObject()->getName())) : NULL);

		for (std::list<TransformHandle>::const_iterator curr = mChildTransforms.begin(); curr != mChildTransforms.end(); ++curr)
		{
			gLogManager.log("		Child: %s", getString((*curr).getObject()->getName()));
		}

		gLogManager.log("		mPosition = (%f,%f,%f)", mPosition.x, mPosition.y, mPosition.z);
		gLogManager.log("		mOrientation = (%f,%f,%f,%f)", mOrientation.x, mOrientation.y, mOrientation.z, mOrientation.w);
		gLogManager.log("		mScale = (%f,%f,%f)", mScale.x, mScale.y, mScale.z);
	}


	/*
	* bool kaleidoscope::Transform::StartUp(const boost::property_tree::ptree& properties)
	*
	* In: ptree : The properties used to start up the transform system
	* Out: bool : true on success.
	*			  false on failure.
	*
	* objects = U32 The maximum number of transforms that can be allocated.
	*/
	bool Transform::StartUp(const boost::property_tree::ptree& properties)
	{
		if (!initialized)
		{
			initialized = true;

			if (!StringIDInit())
			{
				initialized = false;
				return false;
			}

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

			success = Mutex::Mutex_init(&childLock);
			if (success != 0)
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

			TransformPool = new Transform[MAXNUMOBJECTS];

			firstFree = &TransformPool[0];

			for (U32 i = 0; i < MAXNUMOBJECTS - 1; ++i)
			{
				TransformPool[i].mNextInFreeList = &TransformPool[i + 1];
			}

			TransformPool[MAXNUMOBJECTS - 1].mNextInFreeList = NULL;

			mErrorManager = ErrorManager();

			return true;

		}

		return true;
	}


	/*
	* bool kaleidoscope::Transform::ShutDown()
	*
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used to shutdown the transform system.
	*/
	bool Transform::ShutDown()
	{
		if (initialized)
		{
			initialized = false;

			delete[] TransformPool;

			Semaphore::Semaphore_destroy(&createSem);
			Semaphore::Semaphore_destroy(&nameSem);
			Mutex::Mutex_destroy(&childLock);
			Semaphore::Semaphore_destroy(&errorSem);

			return true;
		}

		return true;
	}


	/*
	* kaleidoscope::TransformHandle kaleidoscope::Transform::Create()
	*
	* In: void :
	* Out: TransformHandle : An freshly initialized transform
	*						 TransformHandle::null if the initilization failed 
	*
	* Error handling functions can be used to get the reason the creation failed.
	*/
	TransformHandle Transform::Create()
	{
		Semaphore::Semaphore_wait(&createSem);

		if (firstFree != NULL)
		{
			Transform* const newTrans = firstFree;
			firstFree = newTrans->mNextInFreeList;
			bool b = newTrans->init(GenerateName());

			// Handle init failure.
			if (b == false)
			{
				newTrans->destroy();

				U32 i = (newTrans - &TransformPool[0]);
				TransformPool[i].mNextInFreeList = firstFree;
				firstFree = &TransformPool[i];

				Semaphore::Semaphore_post(&createSem);
				return TransformHandle::null;
			}

			TransformHandle th(newTrans);
			Semaphore::Semaphore_post(&createSem);
			return th;
		}
		else
		{
			setError(CODE_POOL_FULL, "The maximum number of transforms has been allocated, increase the number of transforms specified in properties at StartUp");
			Semaphore::Semaphore_post(&createSem);
			return TransformHandle::null;
		}
	}


	/*
	* kaleidoscope::TransformHandle kaleidoscope::Transform::Create(const boost::property_tree::ptree& transformInfo)
	*
	* In: ptree : The properties to initialize the transform with
	* Out: TransformHandle : An initialized transform
	*						 TransformHandle::null if the initilization failed
	*
	* Error handling functions can be used to get the reason the creation failed.
	*/
	TransformHandle Transform::Create(const boost::property_tree::ptree& transformInfo)
	{
		TransformHandle th = Transform::Create();
		if (th != TransformHandle::null)
		{
			th.getObject()->SerializeIn(transformInfo);
		}
		return th;
	}


	/*
	* void kaleidoscope::Transform::Destroy(const kaleidoscope::TransformHandle& transformToDestroy)
	*
	* In: TransformHandle : The transform to destroy.
	* Out: void :
	*/
	void Transform::Destroy(const TransformHandle& transformToDestroy)
	{
		Semaphore::Semaphore_wait(&createSem);

		Transform* const th = transformToDestroy.getObject();

		if (th != NULL)
		{
			U64 i = (th - &TransformPool[0]);
			TransformPool[i].destroy();
			TransformPool[i].mNextInFreeList = firstFree;
			firstFree = &TransformPool[i];
		}

		Semaphore::Semaphore_post(&createSem);
	}


	/*
	* bool kaleidoscope::Transform::hasPendingError()
	*
	* In: void :
	* Out: bool : true if there is an error.
	*			  false if there isnt an error.
	*
	* If Create throws an error the system logs an error. The error state stays the same until it is either cleared by
	*	the user or Create throws another error. In this case the previous error is overwritten.
	*/
	bool Transform::hasPendingError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		bool b = mErrorManager.hasPendingError();
		Semaphore::Semaphore_post(&errorSem);
		return b;
	}


	/*
	* void kaleidoscope::Transform::clearError()
	*
	* In: void :
	* Out: void :
	*
	* If there was an error in the transform system clear it out.
	* Used after processing an error to tell when the next error occurs.
	*/
	void Transform::clearError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.clearError();
		Semaphore::Semaphore_post(&errorSem);
	}


	/*
	* kaleidoscope::ErrorCode kaleidoscope::Transform::getErrorCode()
	*
	* In: void : 
	* Out: ErrorCode : The error code representing the current state of the transform system.
	*/
	ErrorCode Transform::getErrorCode()
	{
		Semaphore::Semaphore_wait(&errorSem);
		ErrorCode ec = mErrorManager.getErrorCode();
		Semaphore::Semaphore_post(&errorSem);
		return ec;
	}


	/*
	* std::string kaleidoscope::Transform::getErrorString()
	*
	* In: void :
	* Out: ErrorCode : The detailed string explaining why the error happened.
	*/
	std::string Transform::getErrorString()
	{
		Semaphore::Semaphore_wait(&errorSem);
		std::string es = mErrorManager.getErrorString();
		Semaphore::Semaphore_post(&errorSem);
		return es;
	}


	/*
	* void kaleidoscope::Transform::setError(kaleidoscope::ErrorCode ec, const std::string& es)
	*
	* In: ErrorCode : What error happened in the system.
	* In: string : The detailed description of what happened.
	* Out: void :
	*
	* used internally. 
	*/
	void Transform::setError(ErrorCode ec, const std::string& es)
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.setError(ec, es);
		Semaphore::Semaphore_post(&errorSem);
	}



	/*
	* kaleidoscope::StringID kaleidoscope::Transform::GenerateName()
	*
	* In: void : 
	* Out: StringID : The name generated for the next transform.
	*
	* Used internally to make sure each transform has a unique name.
	*/
	U32 Transform::numTrans = 0;
	StringID Transform::GenerateName()
	{
		char buff[25];
		Semaphore::Semaphore_wait(&nameSem);
		I32 n = sprintf_s(buff, "Transform__%u", numTrans++);
		Semaphore::Semaphore_post(&nameSem);
		StringID name = internString(buff);
		return name;
	}


	U32 Transform::MAXNUMOBJECTS;
	StringID Transform::NAME = hashCRC32("transform");

	
	bool Transform::initialized = false;

	const U32 Transform::DEFAULTMAX = 10;

	const math::vec3 Transform::DEFAULTPOSITION(0.0f, 0.0f, 0.0f);
	const math::vec3 Transform::DEFAULTSCALE(1.0f, 1.0f, 1.0f);
	const math::quat Transform::DEFAULTORIENTATION(1.0f, 0.0f, 0.0f, 0.0f);


	Transform* Transform::firstFree = NULL;
	Transform* Transform::TransformPool = NULL;

	ErrorManager Transform::mErrorManager;
}