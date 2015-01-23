#include <Components/Renderable/RenderableHandle.h>
#include <Components/Renderable/Renderable.h>

namespace kaleidoscope
{

	//////////////////////////////////////////////////////////////////////////
	/// Handle Functions.
	//////////////////////////////////////////////////////////////////////////

	const RenderableHandle RenderableHandle::null = RenderableHandle();

	bool RenderableHandle::valid() const { return (getObject() == NULL ? false : true); }

	RenderableHandle::RenderableHandle(const Renderable* c) : mName(c->mName), mPoolIndex(c->mPoolIndex) {}

	Renderable* RenderableHandle::getObject() const
	{
		Renderable* pR = &Renderable::sRenderablePool[mPoolIndex];

		if ((pR != NULL) && (pR->mInitialized) && (pR->mName == mName))
		{
			return pR;
		}
		else
		{
			return NULL;
		}
	}




	//////////////////////////////////////////////////////////////////////////
	/// Renderable Interface.
	//////////////////////////////////////////////////////////////////////////

	void RenderableHandle::SerializeIn(const boost::property_tree::ptree& renderableInfo) { getObject()->SerializeIn(renderableInfo); }
	boost::property_tree::ptree* RenderableHandle::SerializeOut() { return getObject()->SerializeOut(); }

	StringID RenderableHandle::getName() const { return getObject()->getName(); }
	TransformHandle RenderableHandle::transform() { return getObject()->transform(); }

	void RenderableHandle::setMesh(StringID mesh) { getObject()->setMesh(mesh); }
	void RenderableHandle::removeMesh() { getObject()->removeMesh(); }

	bool RenderableHandle::isEnabled() const { return getObject()->visible(); }
	void RenderableHandle::enable() { getObject()->makeVisible(); }
	void RenderableHandle::disable() { getObject()->makeInvisible(); }

	U32 RenderableHandle::numMaterials() const { return getObject()->numMaterials(); }

	void RenderableHandle::setMaterialShader(U32 matNumber, StringID shader){ getObject()->setMaterialShader(matNumber, shader); }
	StringID RenderableHandle::getMaterialShader(U32 matNumber) const { return getObject()->getMaterialShader(matNumber); }

	void RenderableHandle::setMaterialAlbedo(U32 matNumber, StringID path) { getObject()->setMaterialAlbedo(matNumber, path); }
	void RenderableHandle::setMaterialNormalMap(U32 matNumber, StringID path) { getObject()->setMaterialNormalMap(matNumber, path); }

	void RenderableHandle::setMaterialShininess(U32 matNumber, F32 val) { getObject()->setMaterialShininess(matNumber, val); }
	F32 RenderableHandle::getMaterialShininess(U32 matNumber) const { return getObject()->getMaterialShininess(matNumber); }

	void RenderableHandle::setMaterialSpecularColor(U32 matNumber, const math::vec4& c) { getObject()->setMaterialSpecularColor(matNumber, c); }
	math::vec4 RenderableHandle::getMaterialSpecularColor(U32 matNumber) const { return getObject()->getMaterialSpecularColor(matNumber); }


	void RenderableHandle::setAlbedo(StringID albedoTex) { getObject()->setAlbedo(albedoTex); }
	irr::scene::IMeshSceneNode* RenderableHandle::getIrrlichtMesh() { return getObject()->getIrrlichtMesh(); }

	void RenderableHandle::printState() const { getObject()->printState(); }




	//////////////////////////////////////////////////////////////////////////
	/// Static Renderable Interface.
	//////////////////////////////////////////////////////////////////////////

	StringID RenderableHandle::NAME = Renderable::NAME;
	U32 RenderableHandle::MAXNUMOBJECTS = 0;
	const char * RenderableHandle::LUA_TYPE_NAME = Renderable::LUA_TYPE_NAME;

	bool RenderableHandle::StartUp(IrrlichtController* irrControl, const boost::property_tree::ptree& properties)
	{ 
		bool b = Renderable::StartUp(irrControl, properties);
		NAME = Renderable::NAME;
		MAXNUMOBJECTS = Renderable::MAXNUMOBJECTS;
		LUA_TYPE_NAME = Renderable::LUA_TYPE_NAME;
		return b;
	}

	bool RenderableHandle::ShutDown() { return Renderable::ShutDown(); }

	RenderableHandle RenderableHandle::Create(const TransformHandle& th) { return Renderable::Create(th); }
	RenderableHandle RenderableHandle::Create(const TransformHandle& th, const boost::property_tree::ptree& renderableInfo) { return Renderable::Create(th, renderableInfo); }

	void RenderableHandle::Destroy(const RenderableHandle& rh) { Renderable::Destroy(rh); }

	void RenderableHandle::UpdateAll() { Renderable::UpdateAll(); }

	bool RenderableHandle::hasPendingError() { return Renderable::hasPendingError(); }
	void RenderableHandle::clearError() { Renderable::clearError(); }
	ErrorCode RenderableHandle::getErrorCode() { return Renderable::getErrorCode(); }
	std::string RenderableHandle::getErrorString() { return Renderable::getErrorString(); }
}