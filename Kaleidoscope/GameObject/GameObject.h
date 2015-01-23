#pragma once

#include <Utility/Typedefs.h>
#include <Utility/StringID/StringId.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/unordered_map.hpp>
#include <list>
#include <bitset>
#include <GameObject/TagPool.h>

#include <Event/Event.h>

#include <GameObject/GameObjectHandle.h>

#include <Components/Transform/TransformHandle.h>
#include <Components/LuaScript/LuaScript.h>
#include <Components/LuaScript/LuaScriptHandle.h>
#include <Components/Camera/CameraHandle.h>
#include <Components/Renderable/RenderableHandle.h>
#include <Components/Light/LightHandle.h>



namespace kaleidoscope
{
	class GameObject
	{
		// Because bitset is used MAXNUMTAGS must be determined at compile time so it cannot be fed by config file.
		static const U32 MAXNUMTAGS = 64;
		static U32 MAXNUMOBJECTS; 
		static U32 NUMBUCKETS; // REALLY DUMB TO CHANGE THIS AT RUNTIME!!!!!

		friend class GameObjectHandle;

	public:
		static StringID NAME;

	private:
		StringID getName() const { return mName; }; 

		bool enabled() const;
		void enable();
		void disable();

		bool setTag(const StringID tag);
		bool clearTag(const StringID tag);
		bool hasTag(const StringID tag) const;

		GameObjectHandle getParent() const;
		void setParent(GameObjectHandle parent);

		GameObjectHandle getChild(StringID name);
		std::list<GameObjectHandle>* getChildren();
		void addChild(GameObjectHandle child);
		void removeChild(GameObjectHandle child);
		void removeChildInternal(GameObjectHandle child);

		bool addComponent(StringID componentType, StringID fileName = LuaScriptHandle::NAME);
		bool addComponent(StringID componentType, boost::property_tree::ptree scriptInfo);
		bool removeComponent(StringID componentType, StringID fileName = LuaScriptHandle::NAME);

		TransformHandle transform();
	    LuaScriptHandle script(StringID scriptName);
		CameraHandle camera();
		RenderableHandle renderable();
		LightHandle light();

		// State printing should be done through the serializeOut() ptree.
		void printState();
		void printTags();
		void printParent();
		void printChildren();

		// State initialization.
		GameObject();					// Used only by the object pool.
		bool init(const StringID name); // Called when an GameObject is created, a pseudo constructor.
		bool destroy();					// Called before a GameObject is destroyed reset the state of the GameObject for reuse by the object pool.
		~GameObject();					// Symmetry.

		void onEvent(const Event& e);

	public:
		void SerializeIn(const boost::property_tree::ptree& goInfo);
	private:
		boost::property_tree::ptree* SerializeOut(); 


		bool mInitialized;
		union
		{
			class
			{
			public:
				StringID mName;
				bool mStatic;
				bool mEnabled;
				std::bitset<MAXNUMTAGS> mTags;
				I16 mPoolIndex;
				GameObjectHandle mParent;
				std::list<GameObjectHandle> mChildren;

				TransformHandle mTransform;
				std::list<LuaScriptHandle> mScripts;
				CameraHandle mCamera;
				RenderableHandle mRenderable;
				LightHandle mLight;

				U32 mBucket;
			};

			GameObject* mNextInFreeList;
		};
		

	public:
		static bool StartUp(U32 numGameObjects = 10, U32 numBuckets = 4);
		static bool ShutDown();

		static GameObjectHandle Create(const StringID name, bool overwrite = false);
		static GameObjectHandle Create(const boost::property_tree::ptree& objectInfo, bool overwrite = false);

		static void Destroy(GameObjectHandle gameObjectToDestroy);
		static void DestroyImmediate(GameObjectHandle gameObjectToDestroy);

		static void SendEvent(GameObjectHandle recipient, const Event& e);
		static void BroadcastEvent(const Event& e);

		static GameObjectHandle FindByName(const StringID name);
		static GameObjectHandle FindByTag(const StringID tag);
		static std::list<GameObjectHandle>* FindAllWithTag(const StringID tag);
		static std::list<GameObjectHandle>* FindAll();

		static void LoadGameWorld(const char* gameWorldFile, bool overwrite = false); // These functions use plain char* instead of StringID
		static void LoadGameWorld1C(const char* gameWorldFile, bool overwrite = false);
		static void SaveGameWorld(const char* gameWorldFile);					   	  //  because no comparisons are done with them it is only 
		static void SaveDynamicGameWorld(const char* gameWorldFile);		 	      //  for file opening.

		static bool RegisterTag(const StringID tag);
		static bool UnregisterTag(const StringID tag);


	private:
		static void rmvP(GameObjectHandle go);
		static void addP(GameObjectHandle go, GameObjectHandle p);
		static void ppBucket(GameObjectHandle go, I32 v);

		static TagPool sTags;

		static GameObject* sFirstFree;
		static GameObject* sGameObjectPool;

		static boost::unordered_map<StringID, GameObjectHandle> sNameMap; // Acceleration structure for find by name. 
	};
}