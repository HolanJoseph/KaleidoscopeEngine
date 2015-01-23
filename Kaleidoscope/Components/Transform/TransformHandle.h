#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

#include <Debug/ErrorManagement/ErrorCodes.h>

#include <list>
#include <boost/property_tree/ptree.hpp>

namespace kaleidoscope
{
	class Transform;

	class TransformHandle
	{
		friend class Transform;

		//////////////////////////////////////////////////////////////////////////
		/// Handle Functions.
		//////////////////////////////////////////////////////////////////////////

		// Comparison Operations.
		friend inline bool operator==(const TransformHandle& lhs, const TransformHandle& rhs);
		friend inline bool operator!=(const TransformHandle& lhs, const TransformHandle& rhs);

		// Copy Control.
	public:
		TransformHandle(const TransformHandle& h) : mName(h.mName), mPoolIndex(h.mPoolIndex) {};
		inline TransformHandle& operator=(const TransformHandle& rhs);

		static const TransformHandle null;

		TransformHandle() : mName(NULLNAME), mPoolIndex(0) {}; // Null TransformHandle.
		~TransformHandle(){};

		// Conversions.
		TransformHandle(const Transform* t);
		Transform* getObject() const;

		bool valid() const;




		//////////////////////////////////////////////////////////////////////////
		/// Transform Interface.
		//////////////////////////////////////////////////////////////////////////

		void SerializeIn(const boost::property_tree::ptree& transformInfo);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const;

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




		//////////////////////////////////////////////////////////////////////////
		/// Static Transform Interface.
		//////////////////////////////////////////////////////////////////////////

		static StringID NAME;
		static U32 MAXNUMOBJECTS;

		static bool StartUp(const boost::property_tree::ptree& properties);
		static bool ShutDown();

		static TransformHandle Create();
		static TransformHandle Create(const boost::property_tree::ptree& transformInfo);

		static void Destroy(const TransformHandle& transformToDestroy);

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

	TransformHandle& TransformHandle::operator=(const TransformHandle& rhs)
	{
		mName = rhs.mName;
		mPoolIndex = rhs.mPoolIndex;
		return *this;
	};

	bool operator==(const TransformHandle& lhs, const TransformHandle& rhs)
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

	bool operator!=(const TransformHandle& lhs, const TransformHandle& rhs)
	{
		return !(lhs == rhs);
	}
}