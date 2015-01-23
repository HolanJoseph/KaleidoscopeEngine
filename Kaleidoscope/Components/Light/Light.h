#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Debug/ErrorManagement/ErrorCodes.h>
#include <Debug/ErrorManagement/ErrorManager.h>

#include <Rendering/Irrlicht/IrrlichtController.h>
#include <irrlicht.h>

#include <boost/property_tree/ptree.hpp>

#include <Components/Transform/TransformHandle.h>

namespace kaleidoscope
{
	class LightHandle;

	class Light
	{
		friend class LightHandle;

		static StringID NAME;
		static U32 MAXNUMOBJECTS;
		static const char * LUA_TYPE_NAME;


		enum LightType
		{
			POINT_LIGHT,
			SPOT_LIGHT,
			DIRECTIONAL_LIGHT
		};


		bool mInitialized;
		union 
		{
			class
			{
			public:
				StringID mName;
				U32 mPoolIndex;

				irr::scene::ILightSceneNode* mirrLight;

				TransformHandle mTransform;
			};
			Light* mNextInFreeList;
		};


		Light();
		bool init(const StringID name, TransformHandle th);
		bool destroy();
		~Light();

		void SerializeIn(const boost::property_tree::ptree& lightInfo);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const { return mName; };
		TransformHandle transform() { return mTransform; };

		void setVisible();
		void setInvisible();
		bool isVisible() const;

		void setLightType(LightType lt);
		LightType getLightType() const;

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




		static bool StartUp(IrrlichtController* irrControl, const boost::property_tree::ptree& properties);
		static bool ShutDown();

		static LightHandle Create(const TransformHandle& th);

		static void Destroy(const LightHandle& lh);

		static void UpdateAll();

		static bool hasPendingError();
		static void clearError();
		static ErrorCode getErrorCode();
		static std::string getErrorString();
		static void setError(ErrorCode ec, const std::string& es);




		static bool initialized;

		static U32 DEFAULTMAX;

		static U32 numLights;
		static StringID GenerateName();

		static Light* sFirstFree;
		static Light* sLightPool;

		static IrrlichtController* mIrrController;

		static ErrorManager mErrorManager;
	};
}