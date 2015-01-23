#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <Debug/Logging/SDLLogManager.h>

#include <list>

#include <Components/Transform/TransformHandle.h>
#include <Components/LuaScript/LuaScriptHandle.h>
#include <Components/Camera/CameraHandle.h>
#include <Components/Renderable/RenderableHandle.h>


namespace kaleidoscope
{
	class GameObject;

	class GameObjectHandle
	{
		friend class GameObject;

		// Comparison Operations.
		friend inline bool operator==(const GameObjectHandle& lhs, const GameObjectHandle& rhs);
		friend inline bool operator!=(const GameObjectHandle& lhs, const GameObjectHandle& rhs);

		// Copy Control.
	public:
		GameObjectHandle(const GameObjectHandle& h) : mName(h.mName), mOPTindex(h.mOPTindex){};
		inline GameObjectHandle& operator=(const GameObjectHandle& rhs);


		static const GameObjectHandle null;

		GameObjectHandle() : mName(NULLNAME), mOPTindex(0){}; // Null GameObjectHandle
		~GameObjectHandle(){};

		bool valid() const;

		StringID getName() const;

		bool enabled() const;
		void enable();
		void disable();

		bool setTag(const StringID tag);
		bool clearTag(const StringID tag);
		bool hasTag(const StringID tag) const;

		GameObjectHandle getParent() const;
		void setParent(GameObjectHandle parent);

		GameObjectHandle getChild(StringID name) const;
		std::list<GameObjectHandle>* getChildren() const;
		void addChild(GameObjectHandle child);
		void removeChild(GameObjectHandle child);
		void removeChild(StringID childName);

		bool addComponent(I32 componentType, StringID name = 0);
		bool removeComponent(I32 componentType, StringID fileName = 0);

		TransformHandle transform();
		LuaScriptHandle script(const StringID fileName);
		CameraHandle camera();
		RenderableHandle renderable();

		void printState() const;


	private:
		StringID mName;
		I16 mOPTindex;

		GameObjectHandle(const GameObject* go);
	public:
		GameObject* getObject() const;
	};


	GameObjectHandle& GameObjectHandle::operator=(const GameObjectHandle& rhs)
	{
		mName = rhs.mName;
		mOPTindex = rhs.mOPTindex;
		return *this;
	};


	bool operator==(const GameObjectHandle& lhs, const GameObjectHandle& rhs)
	{
		if ((lhs.mName == rhs.mName) && (lhs.mOPTindex == rhs.mOPTindex))
		{
			return true;
		}
		else
		{
			return false;
		}
	}


	bool operator!=(const GameObjectHandle& lhs, const GameObjectHandle& rhs)
	{
		return !(lhs == rhs);
	}
}

