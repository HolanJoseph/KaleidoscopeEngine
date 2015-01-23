#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Debug/ErrorManagement/ErrorCodes.h>

#include <Math/Math.h>

#include <Event/Event.h>

#include <boost/property_tree/ptree.hpp>

namespace kaleidoscope
{
	class LuaScript;
	class GameObjectHandle;

	class LuaScriptHandle
	{
		friend class LuaScript;

		//////////////////////////////////////////////////////////////////////////
		/// Handle Functions.
		//////////////////////////////////////////////////////////////////////////

		// Comparison Operations.
		friend inline bool operator==(const LuaScriptHandle& lhs, const LuaScriptHandle& rhs);
		friend inline bool operator!=(const LuaScriptHandle& lhs, const LuaScriptHandle& rhs);

		// Copy Control.
	public:
		LuaScriptHandle(const LuaScriptHandle& h) : mName(h.mName), mPoolIndex(h.mPoolIndex) {};
		inline LuaScriptHandle& operator=(const LuaScriptHandle& rhs);

		static const LuaScriptHandle null;

		LuaScriptHandle() : mName(NULLNAME), mPoolIndex(0) {}; // Null LUAScriptHandle.
		~LuaScriptHandle(){};

		// Conversions.
		LuaScriptHandle(const LuaScript* t);
		LuaScript* getObject() const;

		bool valid() const;




		//////////////////////////////////////////////////////////////////////////
		/// LuaScript Interface.
		//////////////////////////////////////////////////////////////////////////

		void SerializeIn(const boost::property_tree::ptree& scriptInfo);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const;
		StringID getFileName() const;

		bool isEnabled() const;
		void enable();
		void disable();

		void onEvent(const Event& e);

		// T getT(StringID name)
		F32 getGlobalNumber(const char * name, bool& success);
		bool getGlobalBool(const char * name, bool& success);
		StringID getGlobalStringID(const char * name, bool& success);
		math::vec3 getGlobalvec3(const char * name, bool& success);
		math::vec4 getGlobalvec4(const char * name, bool& success);
		math::mat4 getGlobalmat4(const char * name, bool& success);
		math::quat getGlobalquat(const char * name, bool& success);
		GameObjectHandle getGlobalGameObjectHandle(const char * name, bool& success);

		// void setT(StringID name, T value);
		void setGlobalNumber(const char * name, F32 value);
		void setGlobalBool(const char * name, bool value);
		void setGlobalStringID(const char * name, StringID value);
		void setGlobalvec3(const char * name, const math::vec3& value);
		void setGlobalvec4(const char * name, const math::vec4& value);
		void setGlobalmat4(const char * name, const math::mat4& value);
		void setGlobalquat(const char * name, const math::quat& value);
		void setGlobalGameObjectHandle(const char * name, const GameObjectHandle& value);

		template <class T>
		void addudata(const char * name, const char * udataName, T dataToCopy);

		void printState() const;




		//////////////////////////////////////////////////////////////////////////
		/// Static LuaScript Interface.
		//////////////////////////////////////////////////////////////////////////

		static StringID NAME;

		static bool StartUp(const boost::property_tree::ptree& properties);
		static bool ShutDown();

		static LuaScriptHandle Create(const StringID fileName);
		static LuaScriptHandle Create(const boost::property_tree::ptree& scriptInfo);

		static void Destroy(const LuaScriptHandle& scriptToDestroy);

		static void UpdateAll(F32 dt);

		static void printBuckets();

		static void AddToBucket(const U32 bucket, const LuaScriptHandle& lh);

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

	LuaScriptHandle& LuaScriptHandle::operator=(const LuaScriptHandle& rhs)
	{
		mName = rhs.mName;
		mPoolIndex = rhs.mPoolIndex;
		return *this;
	};

	bool operator==(const LuaScriptHandle& lhs, const LuaScriptHandle& rhs)
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

	bool operator!=(const LuaScriptHandle& lhs, const LuaScriptHandle& rhs)
	{
		return !(lhs == rhs);
	}

	template <class T>
	void LuaScriptHandle::addudata(const char * name, const char * udataName, T dataToCopy)
	{
		getObject()->addudata<T>(name, udataName, dataToCopy);
	}
}