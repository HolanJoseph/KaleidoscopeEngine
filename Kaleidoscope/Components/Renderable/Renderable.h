#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Debug/ErrorManagement/ErrorCodes.h>
#include <Debug/ErrorManagement/ErrorManager.h>

#include <Components/Transform/TransformHandle.h>

#include <Rendering/Irrlicht/IrrlichtController.h>
#include <irrlicht.h>

#include <boost/property_tree/ptree.hpp>

namespace kaleidoscope
{
	class RenderableHandle;

	class Renderable
	{
		friend class RenderableHandle;


		static StringID NAME;
		static U32 MAXNUMOBJECTS;
		static const char * LUA_TYPE_NAME;


		bool mInitialized;
		union 
		{
			class
			{
			public:
				StringID mName;
				U32 mPoolIndex;

				irr::scene::IMeshSceneNode* mirrMesh;
				StringID mMeshPath;
				StringID mAlbedoPath;

				TransformHandle mTransform;
			};
			Renderable* mNextInFreeList;
		};


		Renderable();
		bool init(const StringID name, TransformHandle th);
		bool destroy();
		~Renderable();

		void SerializeIn(const boost::property_tree::ptree& renderableInfo);
		void SerializeMaterialListIn(const boost::property_tree::ptree& matList);
		void SerializeMaterialIn(const boost::property_tree::ptree& mat, U32 matNum);
		boost::property_tree::ptree* SerializeOut();

		StringID getName() const { return mName; };
		TransformHandle transform() { return mTransform; };

		void setMesh(StringID mesh);
		void removeMesh();

		bool visible() const;
		void makeVisible();
		void makeInvisible();

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
		irr::scene::IMeshSceneNode* getIrrlichtMesh() { return mirrMesh; };

		void printState() const;





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
		static void setError(ErrorCode ec, const std::string& es);


		static bool initialized;

		static U32 DEFAULTMAX;

		static U32 numRenderables;
		static StringID GenerateName();

		static Renderable* sFirstFree;
		static Renderable* sRenderablePool;

		static IrrlichtController* mIrrController;

		static ErrorManager mErrorManager;
	};
}