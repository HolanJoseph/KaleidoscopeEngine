#include <Components/Light/Light.h>

#include <Components/Light/LightHandle.h>

#include <Synchronization/Locks/Semaphore/Semaphore.h>

#include <Debug/Logging/SDLLogManager.h>
extern kaleidoscope::SDLLogManager gLogManager;

#include <string>

#include <Utility/Parsing/generateStringFromMaths.h>
#include <Utility/Parsing/parseMathsFromStrings.h>

namespace kaleidoscope
{
	static kaleidoscope::Semaphore createSem;
	static kaleidoscope::Semaphore nameSem;
	static kaleidoscope::Semaphore errorSem;

	Light::Light()
	{
		mInitialized = false;
		mNextInFreeList = NULL;
	}


	/*
	* bool kaleidoscope::Light::init(const kaleidoscope::StringID name, kaleidoscope::TransformHandle th)
	*
	* In: StringID : The internal unique name for the light.
	* In: TransformHandle : The transform to be associated with this light, representing its location.
	* Out: bool : true on success.
	*			  false on failure.
	*
	* Used internally by Light::Create() to initialize a pool object.
	*/
	bool Light::init(const StringID name, TransformHandle th)
	{
		mInitialized = true;

		mName = name;
		mPoolIndex = this - &sLightPool[0];

		mirrLight = mIrrController->addLightSceneNode();
		if (mirrLight == NULL)
		{
			setError(CODE_IRRLICHT_ERROR, "Could not add a scene node to Irrlicht Scene Manager");
			return false;
		}

		mTransform = th;

		return true;
	}


	/*
	* bool kaleidoscope::Light::destroy()
	*
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used internally by Light::Destroy() to reset a pool objects state so it may be reused.
	*/
	bool Light::destroy()
	{
		mIrrController->removeSceneNode(mirrLight);

		mInitialized = false;
		mNextInFreeList = NULL;

		return true;
	}

	Light::~Light(){}


	/*
	* void kaleidoscope::Light::SerializeIn(const boost::property_tree::ptree& lightInfo)
	*
	* In: ptree : property tree representing the lights internal state.
	* Out: void :
	*
	* Fill in the lights state with the values provided by the property tree.
	*
	* <visible>true or false</visible>
	* <light type>point or spot or directional</light type>
	* <radius> number </radius>
	* <ambient color>(r, g, b, a)</ambient color>
	* <diffuse color>(r, g, b, a)</diffuse color>
	* <specular color>(r, g, b, a)</specular color>
	* <attenuation>(quadratic, linear, constant)</attenuation>
	* <inner cone> number </inner cone>
	* <outer cone> number </outer cone>
	* <falloff> number </falloff>
	*/
	void Light::SerializeIn(const boost::property_tree::ptree& lightInfo)
	{
		using boost::property_tree::ptree;

		boost::optional<std::string> visible = lightInfo.get_optional<std::string>("visible");
		boost::optional<std::string> lightType = lightInfo.get_optional<std::string>("light type");
		boost::optional<F32> radius = lightInfo.get_optional<F32>("radius");
		boost::optional<std::string> amCol = lightInfo.get_optional<std::string>("ambient color");
		boost::optional<std::string> difCol = lightInfo.get_optional<std::string>("diffuse color");
		boost::optional<std::string> specCol = lightInfo.get_optional<std::string>("specular color");
		boost::optional<std::string> atten = lightInfo.get_optional<std::string>("attenuation");
		boost::optional<F32> ic = lightInfo.get_optional<F32>("inner cone");
		boost::optional<F32> oc = lightInfo.get_optional<F32>("outer cone");
		boost::optional<F32> fo = lightInfo.get_optional<F32>("falloff");

		if (visible && ((*visible).compare("false") == 0))
		{
			setInvisible();
		}

		if (lightType)
		{
			if ((*lightType).compare("point") == 0)
			{
				setLightType(POINT_LIGHT);
			}
			else if ((*lightType).compare("spot") == 0)
			{
				setLightType(SPOT_LIGHT);
			}
			else if ((*lightType).compare("directional") == 0)
			{
				setLightType(DIRECTIONAL_LIGHT);
			}
		}

		if (radius)
		{
			setRadius(*radius);
		}

		if (amCol)
		{
			bool b;
			math::vec4 ac = stringToVec4((*amCol).c_str(), b);
			setAmbientColor(ac);
		}

		if (difCol)
		{
			bool b;
			math::vec4 dc = stringToVec4((*difCol).c_str(), b);
			setDiffuseColor(dc);
		}

		if (specCol)
		{
			bool b;
			math::vec4 sc = stringToVec4((*specCol).c_str(), b);
			setSpecularColor(sc);
		}

		if (atten)
		{
			bool b;
			math::vec3 att = stringToVec3((*atten).c_str(), b);
			setAttenuation(att);
		}

		if (ic)
		{
			setInnerCone(*ic);
		}

		if (oc)
		{
			setOuterCone(*oc);
		}

		if (fo)
		{
			setFalloff(*fo);
		}

	}


	/*
	* boost::property_tree::ptree* kaleidoscope::Light::SerializeOut()
	*
	* In: void :
	* Out: ptree* : a pointer to a ptree representing the internal state of the light.
	*
	* THE USER IS RESPONSIBILE FOR DELETING THE POINTER.
	*/
	boost::property_tree::ptree* Light::SerializeOut()
	{
		using boost::property_tree::ptree;

		ptree* lI = new ptree();

		lI->add("visible", (isVisible() ? "true" : "false"));
		
		switch (getLightType())
		{
		case POINT_LIGHT:
			lI->add("light type", "point");
			break;
		case SPOT_LIGHT:
			lI->add("light type", "spot");
			break;
		case DIRECTIONAL_LIGHT:
			lI->add("light type", "directional");
			break;
		default:
			break;
		}

		lI->add("radius", getRadius());

		lI->add("ambient color", vec4ToString(getAmbientColor()));
		lI->add("diffuse color", vec4ToString(getDiffuseColor()));
		lI->add("specular color", vec4ToString(getSpecularColor()));

		lI->add("attenuation", vec3ToString(getAttenuation()));

		lI->add("inner cone", getInnerCone());
		lI->add("outer cone", getOuterCone());
		lI->add("falloff", getFalloff());


		return lI;
	}


	/*
	* void kaleidoscope::Light::setVisible()
	*
	* In: void :
	* Out: void :
	*
	*/
	void Light::setVisible() { mirrLight->setVisible(true); }



	/*
	* void kaleidoscope::Light::setInvisible()
	*
	* In: void :
	* Out: void :
	*/
	void Light::setInvisible() { mirrLight->setVisible(false); }


	/*
	* bool kaleidoscope::Light::isVisible()
	*
	* In: void :
	* Out: bool : true if the light is visible.
	*			  false if the light is invisible.
	*/
	bool Light::isVisible() const {return mirrLight->isVisible(); }


	/*
	* void kaleidoscope::Light::setLightType(kaleidoscope::LightType lt)
	*
	* In: LightType : The type of light source this light should represent.
	* Out: void :
	*
	* See enum light type in Light.h
	*/
	void Light::setLightType(LightType lt)
	{
		switch (lt)
		{
		case kaleidoscope::Light::POINT_LIGHT:
			mirrLight->setLightType(irr::video::ELT_POINT);
			break;
		case kaleidoscope::Light::SPOT_LIGHT:
			mirrLight->setLightType(irr::video::ELT_SPOT);
			break;
		case kaleidoscope::Light::DIRECTIONAL_LIGHT:
			mirrLight->setLightType(irr::video::ELT_DIRECTIONAL);
			break;
		default:
			break;
		}
	}


	/*
	* kaleidoscope::Light::LightType kaleidoscope::Light::getLightType() const
	*
	* In: void :
	* Out: LightType : The type of light source the light is currently representing.
	*/
	Light::LightType Light::getLightType() const
	{
		irr::video::E_LIGHT_TYPE lt = mirrLight->getLightType();
		LightType klt;

		switch (lt)
		{
		case irr::video::ELT_POINT:
			klt = POINT_LIGHT;
			break;
		case irr::video::ELT_SPOT:
			klt = SPOT_LIGHT;
			break;
		case irr::video::ELT_DIRECTIONAL:
			klt = DIRECTIONAL_LIGHT;
			break;
		default:
			klt = DIRECTIONAL_LIGHT;
			break;
		}

		return klt;
	}


	/*
	* void kaleidoscope::Light::setRadius(F32 r)
	*
	* In: F32 : The new radius to use if the light is representing a point light.
	* Out:  :
	*/
	void Light::setRadius(F32 r) { mirrLight->setRadius(r); }

	/*
	* F32 kaleidoscope::Light::getRadius() const
	*
	* In: void :
	* Out: F32 : The current radius of the light.
	*/
	F32 Light::getRadius() const { return mirrLight->getRadius(); }


	/*
	* void kaleidoscope::Light::setAmbientColor(const kaleidoscope::math::vec4& ac)
	*
	* In: vec4 : The new color to represent the ambient light given off by the light source.
	* Out: void :
	*/
	void Light::setAmbientColor(const math::vec4& ac) { mirrLight->getLightData().AmbientColor = irr::video::SColorf(ac.x, ac.y, ac.z, ac.w); }


	/*
	* kaleidoscope::math::vec4 kaleidoscope::Light::getAmbientColor()
	*
	* In: void :
	* Out: vec4 : The current ambient color given off by the light source.
 	*/
	math::vec4 Light::getAmbientColor() const
	{
		irr::video::SColorf ic = mirrLight->getLightData().AmbientColor;
		return math::vec4(ic.r, ic.g, ic.b, ic.a);
	}


	/*
	* void kaleidoscope::Light::setDiffuseColor(const kaleidoscope::math::vec4& ac)
	*
	* In: vec4 : The new color to represent the diffuse light given off by the light source.
	* Out: void :
	*/
	void Light::setDiffuseColor(const math::vec4& ac) { mirrLight->getLightData().DiffuseColor = irr::video::SColorf(ac.x, ac.y, ac.z, ac.w); }


	/*
	* kaleidoscope::math::vec4 kaleidoscope::Light::getDiffuseColor() const
	*
	* In: void :
	* Out: vec4 : The current diffuse color given off by the light source.
	*/
	math::vec4 Light::getDiffuseColor() const
	{
		irr::video::SColorf ic = mirrLight->getLightData().DiffuseColor;
		return math::vec4(ic.r, ic.g, ic.b, ic.a);
	}


	/*
	* void kaleidoscope::Light::setSpecularColor(const kaleidoscope::math::vec4& ac)
	*
	* In: vec4 : The new color to represent the specular light given off by the light source.
	* Out: void :
	*/
	void Light::setSpecularColor(const math::vec4& ac) { mirrLight->getLightData().SpecularColor = irr::video::SColorf(ac.x, ac.y, ac.z, ac.w); }


	/*
	* kaleidoscope::math::vec4 kaleidoscope::Light::getSpecularColor() const
	*
	* In: void :
	* Out: vec4 : The current specular color given off by the light source.
	*/
	math::vec4 Light::getSpecularColor() const
	{
		irr::video::SColorf ic = mirrLight->getLightData().SpecularColor;
		return math::vec4(ic.r, ic.g, ic.b, ic.a);
	}


	/*
	* void kaleidoscope::Light::setAttenuation(const kaleidoscope::math::vec3& atten)
	*
	* In: vec3 : The new attenuation values to control the lights falloff specified as
	*			   (quadratic, linear, constant)
	* Out: void :
	*/
	void Light::setAttenuation(const math::vec3& atten) { mirrLight->getLightData().Attenuation = irr::core::vector3df(atten.x, atten.y, atten.z); }


	/*
	* kaleidoscope::math::vec3 kaleidoscope::Light::getAttenuation() const
	*
	* In: void :
	* Out: vec3 : The current attenuation values used to control the lights falloff specified as ]
	*			    (quadratic, linear, constant)
	*/
	math::vec3 Light::getAttenuation() const
	{
		irr::core::vector3df ia = mirrLight->getLightData().Attenuation;
		return math::vec3(ia.X, ia.Y, ia.Z);
	}


	/*
	* void kaleidoscope::Light::setInnerCone(F32 ic)
	*
	* In: F32 : The new angle to use for the inner cone of the light when its representing a spotlight
	* Out: void :
	*/
	void Light::setInnerCone(F32 ic) { mirrLight->getLightData().InnerCone = ic; }


	/*
	* F32 kaleidoscope::Light::getInnerCone() const
	*
	* In: void :
	* Out: F32 :  The angle of the inner cone of the light when its representing a spotlight
	*/
	F32 Light::getInnerCone() const { return mirrLight->getLightData().InnerCone; }


	/*
	* void kaleidoscope::Light::setOuterCone(F32 oc)
	*
	* In: F32 : The new angle to use for the outer cone of the light when its representing a spotlight
	* Out: void :
	*/
	void Light::setOuterCone(F32 oc) { mirrLight->getLightData().OuterCone = oc; }


	/*
	* F32 kaleidoscope::Light::getOuterCone() const
	*
	* In: void :
	* Out: F32 : The angle of the outer cone of the light when its representing a spotlight
	*/
	F32 Light::getOuterCone() const { return mirrLight->getLightData().OuterCone; }


	/*
	* void kaleidoscope::Light::setFalloff(F32 f)
	*
	* In: F32 : The new value to represent the light strengths decrease between outer and inner cone.
	* Out:  :
	*/
	void Light::setFalloff(F32 f) { mirrLight->getLightData().Falloff = f; }


	/*
	* F32 kaleidoscope::Light::getFalloff() const
	*
	* In: void :
	* Out: F32 : The light strengths decrease between outer and inner cone.
	*/
	F32 Light::getFalloff() const { return mirrLight->getLightData().Falloff; }


	/*
	* void kaleidoscope::Light::printState() const
	*
	* In:  void :
	* Out: void :
	*
	* Print the internal state of the transform
	* NEEDS TO BE REPLACED
	*	replace with CSV
	*/
	void Light::printState() const
	{

	}


	/*
	* bool kaleidoscope::Light::StartUp(kaleidoscope::IrrlichtController* irrControl, const boost::property_tree::ptree& properties)
	*
	* In: ptree : The properties used to start up the light subsystem
	* Out: bool : true on success.
	*			  false on failure.
	*
	* objects = U32 The maximum number of lights that can be allocated.
	*/
	bool Light::StartUp(IrrlichtController* irrControl, const boost::property_tree::ptree& properties)
	{
		if (!initialized)
		{
			initialized = true;

			if (!StringIDInit())
			{
				initialized = false;
				return false;
			}

			boost::optional<U32> numObject = properties.get_optional<U32>("objects");
			if (numObject)
			{
				MAXNUMOBJECTS = *numObject;
			}
			else
			{
				MAXNUMOBJECTS = DEFAULTMAX;
			}

			sLightPool = new Light[MAXNUMOBJECTS];

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

			sFirstFree = &sLightPool[0];

			for (U32 i = 0; i < MAXNUMOBJECTS - 1; ++i)
			{
				sLightPool[i].mNextInFreeList = &sLightPool[i + 1];
			}

			sLightPool[MAXNUMOBJECTS - 1].mNextInFreeList = NULL;

			return true;
		}

		return true;
	}


	/*
	* bool kaleidoscope::Light::ShutDown()
	*
	* In: void :
	* Out: bool : Always returns true.
	*
	* Used to shutdown the light subsystem.
	*/
	bool Light::ShutDown()
	{
		if (initialized)
		{
			initialized = false;

			delete[] sLightPool;

			Semaphore::Semaphore_destroy(&createSem);
			Semaphore::Semaphore_destroy(&nameSem);
			Semaphore::Semaphore_destroy(&errorSem);

			return true;
		}

		return true;
	}


	/*
	* kaleidoscope::LightHandle kaleidoscope::Light::Create(const kaleidoscope::TransformHandle& th)
	*
	* In: TransformHandle : The transform to associate with the new renderable.
	* Out: LightHandle : An freshly initialized light.
	*						  LightHandle::null if the initilization failed.
	*
	* Error handling functions can be used to get the reason the creation failed.
	*/
	LightHandle Light::Create(const TransformHandle& th)
	{
		Semaphore::Semaphore_wait(&createSem);

		if (sFirstFree != NULL)
		{
			Light* const newLight = sFirstFree;
			sFirstFree = newLight->mNextInFreeList;
			bool b = newLight->init(GenerateName(), th);

			// Handle init failure.
			if (b == false)
			{
				newLight->destroy();

				U32 i = (newLight - &sLightPool[0]);
				sLightPool[i].mNextInFreeList = sFirstFree;
				sFirstFree = &sLightPool[i];

				Semaphore::Semaphore_post(&createSem);
				return LightHandle::null;
			}

			LightHandle lh(newLight);
			Semaphore::Semaphore_post(&createSem);
			return lh;
		}
		else
		{
			setError(CODE_POOL_FULL, "The maximum number of Lights has been allocated, increase the number of Lights specified in properties at StartUp");
			Semaphore::Semaphore_post(&createSem);
			return LightHandle::null;
		}
	}


	/*
	* void kaleidoscope::Light::Destroy(const kaleidoscope::LightHandle& lh)
	*
	* In: LightHandle : The light to destroy.
	* Out: void :
	*/
	void Light::Destroy(const LightHandle& lh)
	{
		Semaphore::Semaphore_wait(&createSem);

		Light* const l = lh.getObject();

		if (lh.valid())
		{
			U32 i = (l - &sLightPool[0]);
			sLightPool[i].destroy();
			sLightPool[i].mNextInFreeList = sFirstFree;
			sFirstFree = &sLightPool[i];
		}

		Semaphore::Semaphore_post(&createSem);
	}


	/*
	* void kaleidoscope::Light::UpdateAll()
	*
	* In: void :
	* Out: void :
	*
	* Synch the underlying Irrlicht State with the lights state for rendering.
	* Internal use by RenderManager.
	*/
	void Light::UpdateAll()
	{
		for (U32 i = 0; i < MAXNUMOBJECTS; ++i)
		{
			if (sLightPool[i].mInitialized)
			{
				Light* l = &(sLightPool[i]);

				math::vec3 wpos = l->transform().getWorldPosition();
				irr::core::vector3df pos(wpos.x, wpos.y, wpos.z);
				l->mirrLight->setPosition(pos);
				l->mirrLight->updateAbsolutePosition();


				math::quat wori = l->transform().getWorldOrientation();
				math::vec3 weuler = kmath::eulerAnglesNoGimalProtect(wori);
				irr::core::vector3df rot(weuler.x, weuler.y, weuler.z);
				rot = rot * (180.0f / kmath::PI);
				l->mirrLight->setRotation(rot);
				l->mirrLight->updateAbsolutePosition();

				math::vec3 wscale = l->transform().getWorldScale();
				irr::core::vector3df scl(wscale.x, wscale.y, wscale.z);
				l->mirrLight->setScale(scl);
				l->mirrLight->updateAbsolutePosition();
			}
		}
	}


	/*
	* bool kaleidoscope::Light::hasPendingError()
	*
	* In: void :
	* Out: bool : true if there is an error.
	*			  false if there isnt an error.
	*
	* If Create throws an error the system logs an error. The error state stays the same until it is either cleared by
	*	the user or Create throws another error. In this case the previous error is overwritten.
	*/
	bool Light::hasPendingError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		bool b = mErrorManager.hasPendingError();
		Semaphore::Semaphore_post(&errorSem);
		return b;
	}


	/*
	* void kaleidoscope::Light::clearError()
	*
	* In: void :
	* Out: void :
	*
	* If there was an error in the transform system clear it out.
	* Used after processing an error to tell when the next error occurs.
	*/
	void Light::clearError()
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.clearError();
		Semaphore::Semaphore_post(&errorSem);
	}


	/*
	* kaleidoscope::ErrorCode kaleidoscope::Light::getErrorCode()
	*
	* In: void :
	* Out: ErrorCode : The error code representing the current state of the transform system.
	*/
	ErrorCode Light::getErrorCode()
	{
		Semaphore::Semaphore_wait(&errorSem);
		ErrorCode ec = mErrorManager.getErrorCode();
		Semaphore::Semaphore_post(&errorSem);
		return ec;
	}


	/*
	* std::string kaleidoscope::Light::getErrorString()
	*
	* In: void :
	* Out: ErrorCode : The detailed string explaining why the error happened.
	*/
	std::string Light::getErrorString()
	{
		Semaphore::Semaphore_wait(&errorSem);
		std::string es = mErrorManager.getErrorString();
		Semaphore::Semaphore_post(&errorSem);
		return es;
	}


	/*
	* void kaleidoscope::Light::setError(kaleidoscope::ErrorCode ec, const std::string& es)
	*
	* In: ErrorCode : What error happened in the system.
	* In: string : The detailed description of what happened.
	* Out: void :
	*
	* used internally.
	*/
	void Light::setError(ErrorCode ec, const std::string& es)
	{
		Semaphore::Semaphore_wait(&errorSem);
		mErrorManager.setError(ec, es);
		Semaphore::Semaphore_post(&errorSem);
	}


	/*
	* kaleidoscope::StringID kaleidoscope::Light::GenerateName()
	*
	* In: void :
	* Out: StringID : The name generated for the next renderable.
	*
	* Used internally to make sure each renderable has a unique name.
	*/
	StringID Light::GenerateName()
	{
		char buff[25];
		Semaphore::Semaphore_wait(&nameSem);
		I32 n = sprintf_s(buff, "Light__%u", numLights++);
		Semaphore::Semaphore_post(&nameSem);
		StringID name = internString(buff);
		return name;
	}

	const char * Light::LUA_TYPE_NAME = "kaleidoscope.light";
	StringID Light::NAME = hashCRC32("light");
	U32 Light::MAXNUMOBJECTS = 0;

	bool Light::initialized = false;

	U32 Light::DEFAULTMAX = 10;

	U32 Light::numLights = 0;
	Light* Light::sFirstFree = NULL;
	Light* Light::sLightPool = NULL;

	IrrlichtController* Light::mIrrController = NULL;

	ErrorManager Light::mErrorManager;
}