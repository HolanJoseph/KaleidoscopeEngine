#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

#include <Components/Light/Light.h>
#include <Components/Transform/TransformHandle.h>

#include <Rendering/Irrlicht/IrrlichtController.h>

namespace kaleidoscope
{
	class Light;

	class LightHandle
	{
		friend class Light;

		//////////////////////////////////////////////////////////////////////////
		/// Handle Functions.
		//////////////////////////////////////////////////////////////////////////

		// Comparison Operations.
		friend inline bool operator==(const LightHandle& lhs, const LightHandle& rhs);
		friend inline bool operator!=(const LightHandle& lhs, const LightHandle& rhs);

		// Copy Control.
	public:
		LightHandle(const LightHandle& l) : mName(l.mName), mPoolIndex(l.mPoolIndex) {};
		inline LightHandle& operator=(const LightHandle& rhs);

		static const LightHandle null;

		LightHandle() : mName(NULLNAME), mPoolIndex(0) {};
		~LightHandle(){};

		// Conversions.
		LightHandle(const Light* l);
		Light* getObject() const;

		bool valid() const;




		//////////////////////////////////////////////////////////////////////////
		/// Light Interface.
		//////////////////////////////////////////////////////////////////////////

		enum LightType
		{
			POINT_LIGHT = kaleidoscope::Light::POINT_LIGHT,
			SPOT_LIGHT = kaleidoscope::Light::SPOT_LIGHT,
			DIRECTIONAL_LIGHT = kaleidoscope::Light::DIRECTIONAL_LIGHT
		};

		void SerializeIn(const boost::property_tree::ptree& lightInfo);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const;
		TransformHandle transform();

		void enable();
		void disable();
		bool isEnabled() const;

		void setLightType(LightHandle::LightType lt);
		LightHandle::LightType getLightType() const;

		void setRadius(F32 r);
		F32 getRadius() const;

		void setAmbientColor(const math::vec4& ac);
		math::vec4 getAmbientColor() const;

		void setDiffuseColor(const math::vec4& ac);
		math::vec4 getDiffuseColor() const;

		void setSpecularColor(const math::vec4& ac);
		math::vec4 getSpecularColor() const;

		void setAttenuation(const math::vec3& atten);
		math::vec3 getAttenuation() const;

		void setInnerCone(F32 ic);
		F32 getInnerCone() const;

		void setOuterCone(F32 oc);
		F32 getOuterCone() const;

		void setFalloff(F32 f);
		F32 getFalloff() const;

		void printState() const;




		//////////////////////////////////////////////////////////////////////////
		/// Static Light Interface.
		//////////////////////////////////////////////////////////////////////////

		static StringID NAME;
		static U32 MAXNUMOBJECTS;
		static const char * LUA_TYPE_NAME;

		static bool StartUp(IrrlichtController* irrControl, const boost::property_tree::ptree& properties);
		static bool ShutDown();

		static LightHandle Create(const TransformHandle& th);

		static void Destroy(const LightHandle& lh);

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

	LightHandle& LightHandle::operator=(const LightHandle& rhs)
	{
		mName = rhs.mName;
		mPoolIndex = rhs.mPoolIndex;
		return *this;
	};

	bool operator==(const LightHandle& lhs, const LightHandle& rhs)
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

	bool operator!=(const LightHandle& lhs, const LightHandle& rhs)
	{
		return !(lhs == rhs);
	}
}