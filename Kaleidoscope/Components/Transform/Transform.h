#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

#include <Debug/ErrorManagement/ErrorCodes.h>
#include <Debug/ErrorManagement/ErrorManager.h>

#include <Components/Transform/TransformHandle.h>

#include <list>

#include <boost/property_tree/ptree.hpp>

namespace kaleidoscope
{
	class GameObject;

	class Transform
	{
		friend class TransformHandle;

		static StringID NAME;
		static U32 MAXNUMOBJECTS;

		Transform();
		bool init(const StringID name);
		bool destroy();
		~Transform();

		void SerializeIn(const boost::property_tree::ptree& transformInfo);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const { return mName; };

		TransformHandle getParent() const;
		void setParent(const TransformHandle& parent);

		TransformHandle getChild(StringID name);
		std::list<TransformHandle>* getChildren();
		void addChild(const TransformHandle& child);
		void removeChild(const TransformHandle& child);

		math::vec3 getLocalPosition() const;
		void setLocalPosition(const math::vec3& newPos);
		void translate(const math::vec3& translationVector);

		math::vec3 getLocalScale() const;
		F32 getLocalXScale() const;
		F32 getLocalYScale() const;
		F32 getLocalZScale() const;

		void setLocalScale(const math::vec3& newScale);
		void setLocalXScale(F32 newXScale);
		void setLocalYScale(F32 newYScale);
		void setLocalZScale(F32 newZScale);

		math::quat getLocalOrientation() const;
		void setLocalOrientation(const math::quat& newOrientation);
		void rotateAroundAxisLocal(const math::vec3& axis, F32 theta);
		void rotateAroundAxisWorld(const math::vec3& axis, F32 theta);

		math::quat getWorldOrientation() const;
		math::vec3 getWorldScale() const;
		math::vec3 getWorldPosition() const;
		void setWorldPosition(const math::vec3& newPosition);

		void rotateAroundWorldPoint(const math::vec3& pt, const math::vec3& axis, F32 theta);
		void lookAt(const math::vec3& pt, const math::vec3& upHint = math::vec3(0.0f, 1.0f, 0.0f));

		math::mat4 getModelToParentMatrix() const;
		math::mat4 getLocalToWorldMatrix() const;
		math::mat4 getWorldToLocalMatrix() const;

		void printState() const;

		bool mInitialized;
		union
		{
			class
			{
			public:
				StringID mName;
				I16 mPoolIndex;

				TransformHandle mParentTransform;
				std::list<TransformHandle> mChildTransforms;

				math::vec3 mScale;
				math::vec3 mPosition;
				math::quat mOrientation;
			};

			Transform* mNextInFreeList;
		};


	private:
		static bool StartUp(const boost::property_tree::ptree& properties);
		static bool ShutDown();

		static TransformHandle Create();
		static TransformHandle Create(const boost::property_tree::ptree& transformInfo);

		static void Destroy(const TransformHandle& transformToDestroy);

		static bool hasPendingError();
		static void clearError();
		static ErrorCode getErrorCode();
		static std::string getErrorString();
		static void setError(ErrorCode ec, const std::string& es);


	private:
		static bool initialized;

		static const U32 DEFAULTMAX;
		static const math::vec3 DEFAULTPOSITION;
		static const math::vec3 DEFAULTSCALE;
		static const math::quat DEFAULTORIENTATION;

		static U32 numTrans;
		static StringID GenerateName();

		static Transform* firstFree;
		static Transform* TransformPool;

		static ErrorManager mErrorManager;
	};
}