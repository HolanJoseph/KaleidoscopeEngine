#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Math/Math.h>

#include <Debug/ErrorManagement/ErrorCodes.h>

#include <Rendering/Irrlicht/IrrlichtController.h>

#include <Components/Transform/TransformHandle.h>

namespace kaleidoscope
{
	class Renderable;

	class RenderableHandle
	{
		friend class Renderable;

		//////////////////////////////////////////////////////////////////////////
		/// Handle Functions.
		//////////////////////////////////////////////////////////////////////////

		// Comparison Operations.
		friend inline bool operator==(const RenderableHandle& lhs, const RenderableHandle& rhs);
		friend inline bool operator!=(const RenderableHandle& lhs, const RenderableHandle& rhs);

		// Copy Control.
	public:
		RenderableHandle(const RenderableHandle& r) : mName(r.mName), mPoolIndex(r.mPoolIndex) {};
		inline RenderableHandle& operator=(const RenderableHandle& rhs);

		static const RenderableHandle null;

		RenderableHandle() : mName(NULLNAME), mPoolIndex(0) {};
		~RenderableHandle(){};

		// Conversions.
		RenderableHandle(const Renderable* r);
		Renderable* getObject() const;

		bool valid() const;




		//////////////////////////////////////////////////////////////////////////
		/// Renderable Interface.
		//////////////////////////////////////////////////////////////////////////

		void SerializeIn(const boost::property_tree::ptree& renderableInfo);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const;
		TransformHandle transform();

		void setMesh(StringID mesh);
		void removeMesh();

		bool isEnabled() const;
		void enable();
		void disable();

		U32 numMaterials() const;

		void setMaterialShader(U32 matNumber, StringID shader);
		StringID getMaterialShader(U32 matNumber) const;

		void setMaterialAlbedo(U32 matNumber, StringID path);
		void setMaterialNormalMap(U32 matNumber, StringID path);

		void setMaterialShininess(U32 matNumber, F32 val);
		F32 getMaterialShininess(U32 matNumber) const;

		void setMaterialSpecularColor(U32 matNumber, const math::vec4& c);
		math::vec4 getMaterialSpecularColor(U32 matNumber) const;

		void setAlbedo(StringID albedoTex);
		irr::scene::IMeshSceneNode* getIrrlichtMesh();

		void printState() const;




		//////////////////////////////////////////////////////////////////////////
		/// Static Renderable Interface.
		//////////////////////////////////////////////////////////////////////////

		static StringID NAME;
		static U32 MAXNUMOBJECTS;
		static const char * LUA_TYPE_NAME;

		static bool StartUp(IrrlichtController* irrControl, const boost::property_tree::ptree& properties);
		static bool ShutDown();

		static RenderableHandle Create(const TransformHandle& th);
		static RenderableHandle Create(const TransformHandle& th, const boost::property_tree::ptree& renderableInfo);

		static void Destroy(const RenderableHandle& rh);

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

	RenderableHandle& RenderableHandle::operator=(const RenderableHandle& rhs)
	{
		mName = rhs.mName;
		mPoolIndex = rhs.mPoolIndex;
		return *this;
	};

	bool operator==(const RenderableHandle& lhs, const RenderableHandle& rhs)
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

	bool operator!=(const RenderableHandle& lhs, const RenderableHandle& rhs)
	{
		return !(lhs == rhs);
	}
}