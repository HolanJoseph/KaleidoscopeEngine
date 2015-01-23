#include <GameObject/GameObjectHandle.h>
#include <GameObject/GameObject.h>

namespace kaleidoscope
{
	const GameObjectHandle GameObjectHandle::null = GameObjectHandle();

	bool GameObjectHandle::valid() const       { return (getObject() == NULL ? false : true); }

	StringID GameObjectHandle::getName() const { return getObject()->getName(); }

	bool GameObjectHandle::enabled() const { return getObject()->enabled(); }
	void GameObjectHandle::enable()  { getObject()->enable(); }
	void GameObjectHandle::disable() { getObject()->disable(); }

	bool GameObjectHandle::setTag(const StringID tag)       { return getObject()->setTag(tag); }
	bool GameObjectHandle::clearTag(const StringID tag)     { return getObject()->clearTag(tag); }
	bool GameObjectHandle::hasTag(const StringID tag) const { return getObject()->hasTag(tag); }

	GameObjectHandle GameObjectHandle::getParent() const      { return getObject()->getParent(); }
	void GameObjectHandle::setParent(GameObjectHandle parent) { return getObject()->setParent(parent); }

	GameObjectHandle GameObjectHandle::getChild(StringID name) const  { return getObject()->getChild(name); }
	std::list<GameObjectHandle>* GameObjectHandle::getChildren() const { return getObject()->getChildren(); }
	void GameObjectHandle::addChild(GameObjectHandle child)           { getObject()->addChild(child); }
	void GameObjectHandle::removeChild(GameObjectHandle child)        { getObject()->removeChild(child); }
	void GameObjectHandle::removeChild(StringID childName)
	{
		GameObjectHandle goh = GameObject::FindByName(childName);
		(goh != null ? getObject()->removeChild(goh) : goh);
	}

	bool GameObjectHandle::addComponent(I32 componentType, StringID name)        { return getObject()->addComponent(componentType, name); }
	bool GameObjectHandle::removeComponent(I32 componentType, StringID fileName) { return getObject()->removeComponent(componentType, fileName); }

	TransformHandle GameObjectHandle::transform() { return getObject()->transform(); }
	LuaScriptHandle GameObjectHandle::script(const StringID fileName) { return getObject()->script(fileName); }
	CameraHandle GameObjectHandle::camera() { return getObject()->camera(); }
	RenderableHandle GameObjectHandle::renderable() { return getObject()->renderable(); }

	void GameObjectHandle::printState() const{ getObject()->printState(); }


	GameObjectHandle::GameObjectHandle(const GameObject* go) : mName(go->mName), mOPTindex(go->mPoolIndex){}


	GameObject* GameObjectHandle::getObject() const
	{
		GameObject* pGO = &GameObject::sGameObjectPool[mOPTindex];

		if ((pGO != NULL) && (pGO->mInitialized) && (pGO->getName() == mName))
		{
			return pGO;
		}
		else
		{
			return NULL;
		}
	}
}