#include <Components/Light/LightHandle.h>

namespace kaleidoscope
{

	//////////////////////////////////////////////////////////////////////////
	/// Handle Functions.
	//////////////////////////////////////////////////////////////////////////

	const LightHandle LightHandle::null = LightHandle();

	bool LightHandle::valid() const { return (getObject() == NULL ? false : true); }

	LightHandle::LightHandle(const Light* l) : mName(l->mName), mPoolIndex(l->mPoolIndex) {}

	Light* LightHandle::getObject() const
	{
		Light* pL = &Light::sLightPool[mPoolIndex];

		if ((pL != NULL) && (pL->mInitialized) && (pL->mName == mName))
		{
			return pL;
		}
		else
		{
			return NULL;
		}
	}




	//////////////////////////////////////////////////////////////////////////
	/// Light Interface.
	//////////////////////////////////////////////////////////////////////////

	void LightHandle::SerializeIn(const boost::property_tree::ptree& lightInfo) { getObject()->SerializeIn(lightInfo); }
	boost::property_tree::ptree* LightHandle::SerializeOut() { return getObject()->SerializeOut(); }

	StringID LightHandle::getName() const { return getObject()->getName(); }
	TransformHandle LightHandle::transform() { return getObject()->transform(); }

	void LightHandle::enable() { getObject()->setVisible(); }
	void LightHandle::disable() { getObject()->setInvisible(); }
	bool LightHandle::isEnabled() const { return getObject()->isVisible(); }

	void LightHandle::setLightType(LightHandle::LightType lt) { getObject()->setLightType(static_cast<Light::LightType>(lt)); }
	LightHandle::LightType LightHandle::getLightType() const { return static_cast<LightHandle::LightType>(getObject()->getLightType()); }

	void LightHandle::setRadius(F32 r) { getObject()->setRadius(r); }
	F32 LightHandle::getRadius() const { return getObject()->getRadius(); }

	void LightHandle::setAmbientColor(const math::vec4& ac) { getObject()->setAmbientColor(ac); }
	math::vec4 LightHandle::getAmbientColor() const { return getObject()->getAmbientColor(); }

	void LightHandle::setDiffuseColor(const math::vec4& ac) { getObject()->setDiffuseColor(ac); }
	math::vec4 LightHandle::getDiffuseColor() const { return getObject()->getDiffuseColor(); }

	void LightHandle::setSpecularColor(const math::vec4& ac) { getObject()->setSpecularColor(ac); }
	math::vec4 LightHandle::getSpecularColor() const { return getObject()->getSpecularColor(); }

	void LightHandle::setAttenuation(const math::vec3& atten) { getObject()->setAttenuation(atten); }
	math::vec3 LightHandle::getAttenuation() const { return getObject()->getAttenuation(); }

	void LightHandle::setInnerCone(F32 ic) { getObject()->setInnerCone(ic); }
	F32 LightHandle::getInnerCone() const { return getObject()->getInnerCone(); }

	void LightHandle::setOuterCone(F32 oc) { getObject()->setOuterCone(oc); }
	F32 LightHandle::getOuterCone() const { return getObject()->getOuterCone(); }

	void LightHandle::setFalloff(F32 f) { getObject()->setFalloff(f); }
	F32 LightHandle::getFalloff() const { return getObject()->getFalloff(); }

	void LightHandle::printState() const { getObject()->printState(); }




	//////////////////////////////////////////////////////////////////////////
	/// Static Camera Interface.
	//////////////////////////////////////////////////////////////////////////

	StringID LightHandle::NAME = Light::NAME;
	U32 LightHandle::MAXNUMOBJECTS = 0;
	const char * LightHandle::LUA_TYPE_NAME = Light::LUA_TYPE_NAME;

	bool LightHandle::StartUp(IrrlichtController* irrControl, const boost::property_tree::ptree& properties)
	{
		bool b = Light::StartUp(irrControl, properties);
		NAME = Light::NAME;
		MAXNUMOBJECTS = Light::MAXNUMOBJECTS;
		LUA_TYPE_NAME = Light::LUA_TYPE_NAME;
		return b;
	}

	bool LightHandle::ShutDown() { return Light::ShutDown(); }

	LightHandle LightHandle::Create(const TransformHandle& th) { return Light::Create(th); }

	void LightHandle::Destroy(const LightHandle& lh) { Light::Destroy(lh); }

	void LightHandle::UpdateAll() { Light::UpdateAll(); }

	bool LightHandle::hasPendingError() { return Light::hasPendingError(); }
	void LightHandle::clearError() { Light::clearError(); }
	ErrorCode LightHandle::getErrorCode() { return Light::getErrorCode(); }
	std::string LightHandle::getErrorString() { return Light::getErrorString(); }
}