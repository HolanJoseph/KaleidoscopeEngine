#include <Components/LuaScript/LuaScriptHandle.h>
#include <Components/LuaScript/LuaScript.h>

#include <GameObject/GameObjectHandle.h>

namespace kaleidoscope
{

	//////////////////////////////////////////////////////////////////////////
	/// Handle Functions.
	//////////////////////////////////////////////////////////////////////////

	const LuaScriptHandle LuaScriptHandle::null = LuaScriptHandle();

	bool LuaScriptHandle::valid() const { return (getObject() == NULL ? false : true); }

	LuaScriptHandle::LuaScriptHandle(const LuaScript* t) : mName(t->mName), mPoolIndex(t->mPoolIndex) {}

	LuaScript* LuaScriptHandle::getObject() const
	{
		LuaScript* pL = &LuaScript::sLUAScriptPool[mPoolIndex];

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
	/// LuaScript Interface.
	//////////////////////////////////////////////////////////////////////////

	void LuaScriptHandle::SerializeIn(const boost::property_tree::ptree& scriptInfo) { getObject()->SerializeIn(scriptInfo); }
	boost::property_tree::ptree* LuaScriptHandle::SerializeOut() { return getObject()->SerializeOut(); }

	StringID LuaScriptHandle::getName() const { return getObject()->getName(); }
	StringID LuaScriptHandle::getFileName() const { return getObject()->getFileName(); }

	bool LuaScriptHandle::isEnabled() const { return getObject()->enabled(); }
	void LuaScriptHandle::enable()  { getObject()->enable(); }
	void LuaScriptHandle::disable() { getObject()->disable(); }

	void LuaScriptHandle::onEvent(const Event& e) { getObject()->onEvent(e); }

	// T getT(StringID name)
	F32 LuaScriptHandle::getGlobalNumber(const char * name, bool& success) { return getObject()->getGlobalNumber(name, success); }
	bool LuaScriptHandle::getGlobalBool(const char * name, bool& success) { return getObject()->getGlobalBool(name, success); }
	StringID LuaScriptHandle::getGlobalStringID(const char * name, bool& success) { return getObject()->getGlobalStringID(name, success); }
	math::vec3 LuaScriptHandle::getGlobalvec3(const char * name, bool& success) { return getObject()->getGlobalvec3(name, success); }
	math::vec4 LuaScriptHandle::getGlobalvec4(const char * name, bool& success) { return getObject()->getGlobalvec4(name, success); }
	math::mat4 LuaScriptHandle::getGlobalmat4(const char * name, bool& success) { return getObject()->getGlobalmat4(name, success); }
	math::quat LuaScriptHandle::getGlobalquat(const char * name, bool& success) { return getObject()->getGlobalquat(name, success); }
	GameObjectHandle LuaScriptHandle::getGlobalGameObjectHandle(const char * name, bool& success) { return getObject()->getGlobalGameObjectHandle(name, success); }

	// void setT(StringID name, T value);
	void LuaScriptHandle::setGlobalNumber(const char * name, F32 value) { getObject()->setGlobalNumber(name, value); }
	void LuaScriptHandle::setGlobalBool(const char * name, bool value) { getObject()->setGlobalBool(name, value); }
	void LuaScriptHandle::setGlobalStringID(const char * name, StringID value) { getObject()->setGlobalStringID(name, value); }
	void LuaScriptHandle::setGlobalvec3(const char * name, const math::vec3& value) { getObject()->setGlobalvec3(name, value); }
	void LuaScriptHandle::setGlobalvec4(const char * name, const math::vec4& value) { getObject()->setGlobalvec4(name, value); }
	void LuaScriptHandle::setGlobalmat4(const char * name, const math::mat4& value) { getObject()->setGlobalmat4(name, value); }
	void LuaScriptHandle::setGlobalquat(const char * name, const math::quat& value) { getObject()->setGlobalquat(name, value); }
	void LuaScriptHandle::setGlobalGameObjectHandle(const char * name, const GameObjectHandle& value) { getObject()->setGlobalGameObjectHandle(name, value); }

	void LuaScriptHandle::printState() const { getObject()->printState(); }




	//////////////////////////////////////////////////////////////////////////
	/// Static LuaScript Interface.
	//////////////////////////////////////////////////////////////////////////

	StringID LuaScriptHandle::NAME = LuaScript::NAME;

	bool LuaScriptHandle::StartUp(const boost::property_tree::ptree& properties)
	{
		bool b = LuaScript::StartUp(properties);
		NAME = LuaScript::NAME;
		return b;
	}

	bool LuaScriptHandle::ShutDown() { return LuaScript::ShutDown(); }

	LuaScriptHandle LuaScriptHandle::Create(const StringID fileName) { return LuaScript::Create(fileName); }
	LuaScriptHandle LuaScriptHandle::Create(const boost::property_tree::ptree& scriptInfo) { return LuaScript::Create(scriptInfo); }

	void LuaScriptHandle::Destroy(const LuaScriptHandle& scriptToDestroy) { LuaScript::Destroy(scriptToDestroy); }

	void LuaScriptHandle::UpdateAll(F32 dt) { LuaScript::UpdateAll(dt); }

	void LuaScriptHandle::printBuckets() { LuaScript::printBuckets(); }

	void LuaScriptHandle::AddToBucket(const U32 bucket, const LuaScriptHandle& lh) { LuaScript::AddToBucket(bucket, lh); }

	bool LuaScriptHandle::hasPendingError() { return LuaScript::hasPendingError(); }
	void LuaScriptHandle::clearError() { LuaScript::clearError(); }
	ErrorCode LuaScriptHandle::getErrorCode() { return LuaScript::getErrorCode(); }
	std::string LuaScriptHandle::getErrorString() { return LuaScript::getErrorString(); }
}