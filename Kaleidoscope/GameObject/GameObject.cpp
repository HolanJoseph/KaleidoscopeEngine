#include <GameObject/GameObject.h>

#include <Debug/Logging/SDLLogManager.h>
#include <Utility/Config/ConfigManager.h>

#include <Synchronization/Locks/Mutex/Mutex.h>
#include <Synchronization/Locks/Semaphore/Semaphore.h>
#include <Synchronization/Threads/Thread.h>

#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/optional.hpp>
#include <Utility/Parsing/Lowerize.h>
#include <string>

#include <algorithm>

#include <Components/Transform/Transform.h>
#include <Components/LuaScript/LuaScript.h>
#include <Components/Camera/Camera.h>
#include <Components/Renderable/Renderable.h>

extern kaleidoscope::SDLLogManager gLogManager;
extern ConfigManager gConfigManager;

namespace kaleidoscope
{
	static Mutex creationMutex;
	static Mutex tagMutex;
	static Mutex childBucketMutex;

	static Semaphore bucketAccessSem;
	static Semaphore enableSem;


	GameObject::GameObject()
	{
		mInitialized = false;
		mNextInFreeList = NULL;
	}

	// Marks a GameObject in the pool as initialized and sets all of its default values.
	// Used by Create.
	bool GameObject::init(const StringID name)
	{
		mInitialized = true;
		mStatic = false;
		mEnabled = false;
		mName = name;
		mTags = std::bitset<MAXNUMTAGS>();
		mPoolIndex = this - &sGameObjectPool[0];

		mParent = GameObjectHandle::null;
		mChildren = std::list<GameObjectHandle>();

		mTransform = TransformHandle::null;
		addComponent(TransformHandle::NAME);

		mScripts = std::list<LuaScriptHandle>();

		mCamera = CameraHandle::null;
		mRenderable = RenderableHandle::null;
		mLight = LightHandle::null;

		mBucket = 0;

		return true;
	}


	// Marks a GameObject in the pool as uninitialized and releases all of its owned resources.
	// Used by Destroy.
	bool GameObject::destroy()
	{
		setParent(GameObjectHandle::null);
		std::list<GameObjectHandle>* tmplist = getChildren();
		for (std::list<GameObjectHandle>::iterator child = tmplist->begin(); child != tmplist->end(); ++child)
		{
			removeChild(*child);
		}
		delete tmplist;

		if (mTransform.valid())
		{
			removeComponent(TransformHandle::NAME);
		}

		for (std::list<LuaScriptHandle>::iterator scr = mScripts.begin(); scr != mScripts.end();)
		{
			LuaScriptHandle toDel = *(scr++);
			removeComponent(LuaScriptHandle::NAME, toDel.getFileName());
		}

		if (mCamera.valid())
		{
			removeComponent(CameraHandle::NAME);
		}

		if (renderable().valid())
		{
			removeComponent(RenderableHandle::NAME);
		}

		if (light().valid())
		{
			removeComponent(LightHandle::NAME);
		}

		mBucket = 0;

		mInitialized = false;
		mNextInFreeList = NULL;

		return true;
	}


	GameObject::~GameObject(){}


	bool GameObject::enabled() const
	{
		return mEnabled;
	}


	void GameObject::enable()
	{
		mEnabled = true;
		for (std::list<LuaScriptHandle>::iterator scr = mScripts.begin(); scr != mScripts.end(); ++scr)
		{
			(*scr).enable();
		}
		for (std::list<GameObjectHandle>::iterator goh = mChildren.begin(); goh != mChildren.end(); ++goh)
		{
			(*goh).getObject()->enable();
		}
		if (camera().valid())
		{
			// enable cam
		}
		if (renderable().valid())
		{
			renderable().enable();
		}
		if (light().valid())
		{
			light().enable();
		}
	}


	void GameObject::disable()
	{
		mEnabled = false; 
		for (std::list<LuaScriptHandle>::iterator scr = mScripts.begin(); scr != mScripts.end(); ++scr)
		{
			(*scr).disable();
		}
		for (std::list<GameObjectHandle>::iterator goh = mChildren.begin(); goh != mChildren.end(); ++goh)
		{
			(*goh).getObject()->disable();
		}
		if (camera().valid())
		{
			// disable cam
		}
		if (renderable().valid())
		{
			renderable().disable();
		}
		if (light().valid())
		{
			light().disable();
		}
	}


	/*
	 * GameObject::setTag(const StringID tag)
	 * 
	 * Attempt to set the provided tag for the GameObject.
	 *
	 * Return Value: true  - the tag has been registered
	 *				 false - the tag has not been registered
	 */
	bool GameObject::setTag(const StringID tag)
	{
		const I8 offset = sTags[tag];

		if (offset != -1)
		{
			mTags.set(offset, true);
			return true;
		}
		return false;
	}


   /*
	* GameObject::clearTag(const StringID tag)
	*
	* Attempt to clear the provided tag for the GameObject.
	*
	* Return Value: true  - the tag has been registered, and has been successfully cleared
	*			    false - the tag has not been registered
	*/
	bool GameObject::clearTag(const StringID tag)
	{
		const I8 offset = sTags[tag];

		if (offset != -1)
		{
			mTags.set(offset, false);
			return true;
		}

		return false;
	}


   /*
	* GameObject::hasTag(const StringID tag)
	*
	* Check if the GameObject has be provided tag
	*
	* Return Value: true  - the GameObject has the provided tag.
	*			    false - the GameObject does not have the provided tag or the tag has not been registered.
	*/
	bool GameObject::hasTag(const StringID tag) const
	{
		const I8 offset = sTags[tag];

		if (offset != -1)
		{
			return mTags.test(offset);
		}

		return false;
	}


	/*
	 * GameObject::getParent() const 
	 *
	 * Return Value: return a copy of the GameObjectHandle corresponding to this GameObjects parent.
	 *				 GameObjectHandle::null will be returned if this GameObject has no parent.
	 */
	GameObjectHandle GameObject::getParent() const
	{
		return mParent;
	}


	/*
	 * GameObject::setParent(GameObjectHandle parent)
	 * 
	 * Set the parent GameObject of this GameObject.
	 */
	void GameObject::setParent(GameObjectHandle parent)
	{
		if (parent == GameObjectHandle::null && getParent().valid())
		{
			rmvP(GameObjectHandle(this));
		}
		else if (!parent.valid())
		{
			return;
		}
		else
		{
			if (getParent().valid())
			{
				rmvP(GameObjectHandle(this));
			}
			addP(GameObjectHandle(this), parent);
		}

	}


   /*
	* GameObject::getChild(StringID name)
	*
	* Return Value: GameObjectHandle::null if this GameObject has no child with the specified name.
	*               A GameObjectHandle corresponding to the child GameObject 
	*/
	GameObjectHandle GameObject::getChild(StringID name)
	{
		GameObjectHandle child;

		for (std::list<GameObjectHandle>::iterator curr = mChildren.begin(); curr != mChildren.end(); ++curr)
		{
			if ((*curr).getObject()->getName() == name)
			{
				child = *curr;
				break;
			}
		}
		return child;
	}


   /*
	* GameObject::getChildren()
	*
	* IT IS THE USERS RESPONSIBILITY TO FREE THE LIST WHEN THEY ARE DONE USING IT.
	*
	* Return Value: A pointer to a list made up of GameObjectHandles corresponding to each of this GameObjects children.
	*/
	std::list<GameObjectHandle>* GameObject::getChildren()
	{
		std::list<GameObjectHandle>* mcp = new std::list<GameObjectHandle>(mChildren.cbegin(), mChildren.cend());
		return mcp;
	}


   /*
	* GameObject::addChild(GameObjectHandle child)
	* 
	* If the provided GameObjectHandle is valid and the GameObject isn't in the highest update bucket
	*   then child will be added to the GameObjects list of children.
	*/
	void GameObject::addChild(GameObjectHandle child)
	{
		if (child.valid())
		{
			if (child.getParent().valid())
			{
				rmvP(child);
			}
			addP(child, GameObjectHandle(this));
		}
	}


   /*
	* GameObject::removeChild(GameObjectHandle child)
	* 
	* If child is a child of the GameObject the child will be remove from its list of children and child
	*   will have its parent set to null.
	*/
	void GameObject::removeChild(GameObjectHandle child)
	{
		if (child.valid() && child.getParent().valid() && child.getParent() == GameObjectHandle(this))
		{
			rmvP(child);
		}
	}


   /*
	* GameObject::addComponent(I32 componentType, StringID name = LUAScript::NAME)
	*
	* Attempt to add the requested component type to this GameObject.
	* The name field is used to provide the file path if the component is of type LUAScriptComponent.
	*
	* Return Value: true  - the component was successfully added.
	*				false - the component was not added either because the GameObject already has a component of that type or because no resources could be allocated.
	*/
	bool GameObject::addComponent(StringID componentType, StringID fileName)
	{

		if (componentType == TransformHandle::NAME && !mTransform.valid())
		{
			mTransform = TransformHandle::Create();
			if (mTransform != TransformHandle::null)
			{
				GameObjectHandle p = getParent();
				if (p.valid() && p.transform().valid())
				{
					getParent().transform().addChild(mTransform);
				}

				for (std::list<GameObjectHandle>::iterator go = mChildren.begin(); go != mChildren.end(); ++go)
				{
					if ((*go).valid() && (*go).transform().valid())
					{
						transform().addChild((*go).transform());
					}
				}

				return true;
			}
		}
		else if (componentType == LuaScriptHandle::NAME)
		{
			if (fileName != LuaScriptHandle::NAME && fileName != 0 && !mStatic)
			{
				LuaScriptHandle lh = LuaScriptHandle::Create(fileName);
				if (lh != LuaScriptHandle::null)
				{
					lh.addudata("this", "kaleidoscope.GameObjectHandle", GameObjectHandle(this));
					mScripts.push_back(lh);
					if (enabled())
					{
						lh.enable();
					}
				}
			}
		}
		else if (componentType == CameraHandle::NAME && !mCamera.valid())
		{
			mCamera = CameraHandle::Create(transform());
		}
		else if (componentType == RenderableHandle::NAME && !renderable().valid())
		{
			mRenderable = RenderableHandle::Create(transform());
		}
		else if (componentType == LightHandle::NAME && !light().valid())
		{
			mLight = LightHandle::Create(transform());
		}

		return false;
	}

	bool GameObject::addComponent(StringID componentType, boost::property_tree::ptree scriptInfo)
	{
		if (componentType == LuaScriptHandle::NAME && !mStatic)
		{
			LuaScriptHandle lh = LuaScriptHandle::Create(scriptInfo);
			if (lh != LuaScriptHandle::null)
			{
				if (!script(lh.getFileName()).valid())
				{
					lh.addudata("this", "kaleidoscope.GameObjectHandle", GameObjectHandle(this));
					mScripts.push_back(lh);
					if (enabled())
					{
						lh.enable();
					}
					return true;
				}
				else
				{
					LuaScriptHandle::Destroy(lh);
				}
			}
		}
		return false;
	}


   /*
	* GameObject::removeComponent(I32 componentType, StringID name = LUAScript::NAME)
	* 
	* Attempt to remove the specified component from this GameObject.
	* name is used to differentiate between attached LUAScripts.
	* 
	* Return Value: true  - the component was successfully removed.
	*				false - this GameObject does not have a component corresponding to the provided component type, so none could be removed.
	*/
	bool GameObject::removeComponent(StringID componentType, StringID fileName)
	{
		bool s = false;

		if (componentType == TransformHandle::NAME && mTransform.valid())
		{
			GameObjectHandle p = getParent();
			if (p.valid() && p.transform().valid())
			{
				transform().setParent(TransformHandle::null);
			}

			for (std::list<GameObjectHandle>::iterator go = mChildren.begin(); go != mChildren.end(); ++go)
			{
				if ((*go).valid() && (*go).transform().valid())
				{
					(*go).transform().setParent(TransformHandle::null);
				}
			}

			TransformHandle::Destroy(mTransform);
			mTransform = TransformHandle::null;
			s = true;
		}
		else if (componentType == LuaScriptHandle::NAME)
		{
			for (std::list<LuaScriptHandle>::iterator ls = mScripts.begin(); ls != mScripts.end(); ++ls)
			{
				if ((*ls).valid() && fileName != LuaScriptHandle::NAME && fileName != 0 && (*ls).getFileName() == fileName)
				{
					LuaScriptHandle lh = (*ls);
					mScripts.erase(ls);

					LuaScriptHandle::Destroy(lh);
					s = true;
					break;
				}
			}
		}
		else if (componentType == CameraHandle::NAME && camera().valid())
		{
			CameraHandle::Destroy(camera());
			mCamera = CameraHandle::null;
			s = true;
		}
		else if (componentType == RenderableHandle::NAME && renderable().valid())
		{
			RenderableHandle::Destroy(renderable());
			mRenderable = RenderableHandle::null;
			s = true;
		}
		else if (componentType == LightHandle::NAME && light().valid())
		{
			LightHandle::Destroy(light());
			mLight = LightHandle::null;
			s = true;
		}

		return s;
	}


   /*
	* GameObject::transform()
	*
	* Return Value: return a TransformHandle corresponding to this GameObjects transform
	*				TransfromHandle::null is returned if this GameObject does not have a transform.
	*/
	TransformHandle GameObject::transform(){ return mTransform; }


   /*
	* GameObject::script(StringID scriptName)
	*
	* Return Value: return a LUAScriptHandle corresponding to the LUAScript with the provided name.
	*				return LUAScriptHandle::null if this GameObject does not have a LUAScript component with the provided name.
	*/
	LuaScriptHandle GameObject::script(StringID scriptName)
	{
		LuaScriptHandle retval = LuaScriptHandle::null;

		for (std::list<LuaScriptHandle>::iterator scr = mScripts.begin(); scr != mScripts.end(); ++scr)
        {
            if ((*scr).valid() && (*scr).getFileName() == scriptName)
            {
				retval = *scr;
            }
			break;
        }
        
		return retval;
	}


	/*
	* GameObject::camera()
	*
	* Return Value: return a CameraHandle corresponding to this GameObjects camera
	*				CameraHandle::null is returned if this GameObject does not have a camera.
	*/
	CameraHandle GameObject::camera()
	{
		if (mCamera.valid())
		{
			return mCamera;
		}
		else
		{
			return CameraHandle::null;
		}
	}


	/*
	* GameObject::renderable()
	*
	* Return Value: return a RenderableHandle corresponding to this GameObjects renderable
	*				RenderableHandle::null is returned if this GameObject does not have a renderable.
	*/
	RenderableHandle GameObject::renderable()
	{
		if (mRenderable.valid())
		{
			return mRenderable;
		} 
		else
		{
			return RenderableHandle::null;
		}
	}


	/*
	* GameObject::renderable()
	*
	* Return Value: return a LightHandle corresponding to this GameObjects light
	*				LightHandle::null is returned if this GameObject does not have a light.
	*/
	LightHandle GameObject::light()
	{
		if (mLight.valid())
		{
			return mLight;
		}
		else
		{
			return LightHandle::null;
		}
	}


   /*
	* GameObject::printState()
	*
	* Print a readout of the internal state of this GameObject.
	*/
	void GameObject::printState()
	{
		gLogManager.log("GameObject %s\n", getString(getName()));
		gLogManager.log("	Static: %s", (mStatic ? "True" : "False"));
		printTags();
		printParent();
		printChildren();
		(mTransform.valid() ? gLogManager.log("	Transform"),gLogManager.log("	["),mTransform.printState(),gLogManager.log("	]") : NULL);

		for (std::list<LuaScriptHandle>::iterator lh = mScripts.begin(); lh != mScripts.end(); ++lh)
		{
			gLogManager.log("	Script"); gLogManager.log("	[");
			(*lh).printState();
			gLogManager.log("	]");
		}

		if (camera().valid())
		{
			gLogManager.log("	Camera"); gLogManager.log("	[");
			camera().printState();
			gLogManager.log("	]");
		}

		if (renderable().valid())
		{
			gLogManager.log("	Renderable"); gLogManager.log("	[");
			renderable().printState();
			gLogManager.log("	]");
		}

		if (light().valid())
		{
			gLogManager.log("	Light"); gLogManager.log("	[");
			light().printState();
			gLogManager.log("	]");
		}

		gLogManager.log("	Update Bucket: %u", mBucket);

	}


	void GameObject::printTags()
	{
		for (TagPool::const_iterator tag = sTags.cbegin(); tag != sTags.cend(); ++tag)
		{
			if (hasTag((*tag).first))
			{
				gLogManager.log("	Tag: %s", getString((*tag).first));
			}
		}

	}


	void GameObject::printParent()
	{
		gLogManager.log("	Parent: %s", (mParent != GameObjectHandle::null ? getString(mParent.getObject()->getName()) : "Null"));
	}


	void GameObject::printChildren()
	{
		for (std::list<GameObjectHandle>::iterator go = mChildren.begin(); go != mChildren.end(); ++go)
		{
			gLogManager.log("	Child: %s", getString((*go).getObject()->getName()));
		}
	}


   /*
	* GameObject::onEvent(const Event& e)
	* 
	* Send the event on to each of this GameObjects LUAScript components to be dealt with.
	*/
	void GameObject::onEvent(const Event& e)
	{
		for (std::list<LuaScriptHandle>::iterator script = mScripts.begin(); script != mScripts.end(); ++script)
		{
			(*script).onEvent(e);
		}
	}


	static const StringID tagID = hashCRC32("tag");
	static const StringID parentID = hashCRC32("parent");
	static const StringID childID = hashCRC32("child");


   /*
	* GameObject::SerializeIn(const boost::property_tree::ptree& goInfo)
	*
	* Use the provided ptree to fill in information about this GameObject, add components, set tags ... etc
	*/
	void GameObject::SerializeIn(const boost::property_tree::ptree& goInfo)
	{
		using boost::property_tree::ptree;

		boost::optional<const ptree&> attrs = goInfo.get_child_optional("<xmlattr>");
		if (attrs)
		{
			boost::optional<bool> stat = attrs->get_optional<bool>("static");
			if (stat)
			{
				mStatic = *stat;
			}
		}

		boost::optional<const ptree&> transInfo = goInfo.get_child_optional("transform");
		if (transInfo)
		{
			removeComponent(TransformHandle::NAME);
			addComponent(TransformHandle::NAME);
			mTransform.SerializeIn(*transInfo);
		}


		StringID fieldID;
		BOOST_FOREACH(ptree::value_type const& goInfoField, goInfo)
		{
			fieldID = lowerize(goInfoField.first.c_str());

			if (fieldID == tagID)
			{
				setTag(internString(goInfoField.second.data().c_str()));
			}
			else if (fieldID == parentID)
			{
				setParent(FindByName(internString(goInfoField.second.data().c_str())));
			}
			else if (fieldID == childID)
			{
			}
			else if (fieldID == LuaScriptHandle::NAME && !mStatic)
			{
				addComponent(LuaScriptHandle::NAME, goInfoField.second);
			}
			else if (fieldID == CameraHandle::NAME && !camera().valid())
			{
				removeComponent(CameraHandle::NAME);
				addComponent(CameraHandle::NAME);
				if (camera().valid())
				{
					camera().SerializeIn(goInfoField.second);
				}
			}
			else if (fieldID == RenderableHandle::NAME && !renderable().valid())
			{
				addComponent(RenderableHandle::NAME);
				if (renderable().valid())
				{
					renderable().SerializeIn(goInfoField.second);
				}
			}
			else if (fieldID == LightHandle::NAME && !light().valid())
			{
				addComponent(LightHandle::NAME);
				if (light().valid())
				{
					light().SerializeIn(goInfoField.second);
				}
			}
		}
	}


   /*
	* boost::property_tree::ptree* GameObject::SerializeOut()
	*
	* Used internally to get a data representation of a GameObject for printing to GameWorld files.
	* 
	* Return Value: a pointer to a ptree containing formatted information representing this GameObject. 
	*/
	// If a value matches default values we may want to exclude it from being serialized out.
	boost::property_tree::ptree* GameObject::SerializeOut()
	{
		using boost::property_tree::ptree;
		ptree* goI = new ptree();
		goI->add("<xmlattr>.static", (mStatic ? "true" : "false"));
		goI->add("name", getString(getName()));

		for (TagPool::const_iterator tag = sTags.cbegin(); tag != sTags.cend(); ++tag)
		{
			if (hasTag((*tag).first))
			{
				goI->add("tag", getString((*tag).first));
			}
		}

		if (mParent != GameObjectHandle::null)
		{
			goI->add("parent", getString(mParent.getObject()->getName()));
		}

		if (mTransform != TransformHandle::null)
		{
			goI->add_child(getString(TransformHandle::NAME), *(mTransform.SerializeOut()));
		}

		for (std::list<LuaScriptHandle>::iterator lsh = mScripts.begin(); lsh != mScripts.end(); ++lsh)
		{
			goI->add_child(getString(LuaScriptHandle::NAME), *((*lsh).SerializeOut()));
		}

		if (camera().valid())
		{
			goI->add_child(getString(CameraHandle::NAME), *(camera().SerializeOut()));
		}

		if (renderable().valid())
		{
			goI->add_child(getString(RenderableHandle::NAME), *(renderable().SerializeOut()));
		}

		if (light().valid())
		{
			goI->add_child(getString(LightHandle::NAME), *(light().SerializeOut()));
		}


		return goI;
	}




   /*
	* GameObject::StartUp(U32 numBuckets)
	*
	* The function that initializes all of the necessary components so that the GameObject system can work.
	* This should be called once before you start using anything related to the GameObject system.
	*
	* Return Value: true  - all initializations were successful.
	*				false - some part of the initialization failed.
	*/
	bool GameObject::StartUp(U32 numGameObjects, U32 numBuckets)
	{
		MAXNUMOBJECTS = numGameObjects;
		NUMBUCKETS = numBuckets;


		boost::optional<const boost::property_tree::ptree&> transformProperties = gConfigManager.getPtree("transform");
		if (transformProperties)
		{
			kaleidoscope::TransformHandle::StartUp(*transformProperties);
		}
		else
		{
			kaleidoscope::TransformHandle::StartUp(boost::property_tree::ptree());
		}



		boost::optional<const boost::property_tree::ptree&> lsProperites = gConfigManager.getPtree("LuaScript");
		if (lsProperites)
		{
			kaleidoscope::LuaScriptHandle::StartUp(*lsProperites);
		}
		else
		{
			kaleidoscope::LuaScriptHandle::StartUp(boost::property_tree::ptree());
		}


		sGameObjectPool = new GameObject[MAXNUMOBJECTS];

		sFirstFree = &sGameObjectPool[0];

		for (U32 i = 0; i < MAXNUMOBJECTS - 1; ++i)
		{
			sGameObjectPool[i].mNextInFreeList = &sGameObjectPool[i + 1];
		}

		sGameObjectPool[MAXNUMOBJECTS - 1].mNextInFreeList = NULL;

		if ((Mutex::Mutex_init(&creationMutex) != 0) || (Mutex::Mutex_init(&tagMutex) != 0) || (Mutex::Mutex_init(&childBucketMutex) != 0) || (Semaphore::Semaphore_init(&bucketAccessSem, 1) != 0) || (Semaphore::Semaphore_init(&enableSem, 1) != 0) )
		{
			return false;
		}

		return true;
	}


   /*
	* GameObject::ShutDown()
	*
	* Free all dynamically allocated resources used by the GameObject system.
	*/
	bool GameObject::ShutDown()
	{
		delete[] sGameObjectPool;
		
		Mutex::Mutex_destroy(&creationMutex);
		Mutex::Mutex_destroy(&tagMutex);
		Mutex::Mutex_destroy(&childBucketMutex);

		kaleidoscope::LuaScriptHandle::ShutDown();
		kaleidoscope::TransformHandle::ShutDown();


		return true;
	}


   /*
	* GameObject::Create(const StringID name, bool overwrite = false)
	*
	* Attempt to create a new GameObject with the requested name.
	* If overwrite is false GameObjectHandle::null will be returned if a GameObject with the requested name already exists.
	*	if overwrite is true the GameObject with the requested name will be destroyed and a new one with the requested name will be created.
	*
	* Return Value: a GameObjectHandle corresponding to the newly created GameObject.
	*				GameObjectHandle::null if no resources are available or a GameObject with the requested name already exists.
	*/
	GameObjectHandle GameObject::Create(const StringID name, bool overwrite)
	{
		// Currently I have no way of reporting warnings from inside the create function as SDL does not
		//	allow SDL function calls in worker threads. So until I determine a way to report errors the 
		//	only warning to Create is the returned Handle which will be null if the call failed.

		if (name == NULLNAME)
		{
			gLogManager.log("WARNING: Cannot create a GameObject with a reserved name.");
			return GameObjectHandle::null;
		}

		Mutex::Mutex_lock(&creationMutex);
		if (sFirstFree != NULL)
		{
			GameObjectHandle nt = FindByName(name);
			if (nt == GameObjectHandle::null || (overwrite && nt.valid() && nt.getName() == name))
			{
				if (nt.valid())
				{
					GameObject::DestroyImmediate(nt);
				}

				GameObject* const newGO = sFirstFree;
				sFirstFree = newGO->mNextInFreeList;

				newGO->init(name);
				GameObjectHandle goh(newGO);
				sNameMap[newGO->getName()] = goh;
				Mutex::Mutex_unlock(&creationMutex);
				return goh;
			}
			else
			{
				// WARNING: A GAMEOBJECT WITH THIS NAME ALREADY EXISTS!

				Mutex::Mutex_unlock(&creationMutex);
				return GameObjectHandle::null;
			}
		}
		else
		{
			// WARNING: NO SPACE AVAILABLE TO CREATE A NEW GAMEOBJECT!

			Mutex::Mutex_unlock(&creationMutex);
			return GameObjectHandle::null;
		}
	}


   /*
	* GameObject::Create(const boost::property_tree::ptree& objectInfo, bool overwrite)
	*
	* a ptree representing data to be serialized into a new GameObject.
	* whether or not this GameObject should delete a GameObject with the same name.
	*
	* Return Value: A GameObjectHandle corresponding to the newly created GameObject.
	*				GameObjectHandle::null if the initilization failed.
	*/
	GameObjectHandle GameObject::Create(const boost::property_tree::ptree& objectInfo, bool overwrite)
	{
		boost::optional<std::string> name = objectInfo.get_optional<std::string>("name");

		// If we cannot find a name field in the ptree.
		if (!name)
		{
			return GameObjectHandle::null;
		}
		else
		{
			const StringID nameID = internString(name->c_str());
			GameObjectHandle goh = Create(nameID, overwrite);
			if (goh == GameObjectHandle::null)
			{
				return GameObjectHandle::null;
			}
			else
			{
				goh.getObject()->SerializeIn(objectInfo);
				return goh;
			}
		}
	}


   /*
	* GameObject::Destroy(GameObjectHandle gameObjectToDestroy)
	*
	* It is likely that at some point immediately destroying gameobjects will result is state errors, currently 
	*	this is not an issue but this will facilitate easy change over if/ when this happens.
	*
	* If the provided GameObjectHandle is valid its corresponding GameObject is destroyed.
	*/
	void GameObject::Destroy(GameObjectHandle gameObjectToDestroy)
	{
		GameObject::DestroyImmediate(gameObjectToDestroy);
	}


   /*
	* GameObject::DestroyImmediate(GameObjectHandle gameObjectToDestroy)
	*
	* If the provided GameObjectHandle is valid its corresponding GameObject is destroyed.
	*/
	void GameObject::DestroyImmediate(GameObjectHandle gameObjectToDestroy)
	{
		Mutex::Mutex_lock(&creationMutex);
		GameObject* const gotd = gameObjectToDestroy.getObject();

		if (gotd != NULL)
		{
			U64 i = (gotd - &sGameObjectPool[0]);
			sNameMap.erase(gotd->getName());

			std::list<GameObjectHandle>* gotdChildren = gotd->getChildren();
			for (std::list<GameObjectHandle>::iterator chld = gotdChildren->begin(); chld != gotdChildren->end(); ++chld)
			{
				GameObject::Destroy(*chld);
			}

			sGameObjectPool[i].destroy();
			sGameObjectPool[i].mNextInFreeList = sFirstFree;
			sFirstFree = &sGameObjectPool[i];
		}
		Mutex::Mutex_unlock(&creationMutex);
	}


   /*
	* GameObject::SendEvent(GameObjectHandle recipient, const Event& e)
	* 
	* Send the provided event to the requested GameObject to be dealt with.
	*/
	void GameObject::SendEvent(GameObjectHandle recipient, const Event& e)
	{
		GameObject* const rGO = recipient.getObject();
		if (rGO != NULL)
		{
			rGO->onEvent(e);
		}
	}


   /*
	* GameObject::BroadcastEvent(const Event& e)
	*
	* Send the provided event to every GameObject to be dealt with.
	*/
	void GameObject::BroadcastEvent(const Event& e)
	{
		for (U64 i = 0; i < MAXNUMOBJECTS; ++i)
		{
			if (sGameObjectPool[i].mInitialized)
			{
				sGameObjectPool[i].onEvent(e);
			}
		}
	}


   /*
	* GameObject::FindByName(const StringID name)
	*
	* Return Value: return a GameObjectHandle corresponding to the GameObject with the requested name if it exists.
	*				GameObjectHandle::null otherwise.
	*/
	GameObjectHandle GameObject::FindByName(const StringID name)
	{
		if (sNameMap.count(name) >= 1)
		{
			return sNameMap[name];
		} 
		else
		{
			return GameObjectHandle::null;
		}
	}


   /*
	* GameObject::FindByTag(const StringID tag)
	*
	* Return Value: return a GameObjectHandle corresponding to the first GameObject found with the requested tag if one exists.
	*				GameObjectHandle::null otherwise.
	*/
	GameObjectHandle GameObject::FindByTag(const StringID tag)
	{
		if (sTags[tag] != -1)
		{
			for (U64 i = 0; i < MAXNUMOBJECTS; ++i)
			{
				if (sGameObjectPool[i].mInitialized && sGameObjectPool[i].hasTag(tag))
				{
					return GameObjectHandle(&sGameObjectPool[i]);
				}
			}
		}

		return GameObjectHandle::null;
	}


   /*
	* GameObject::FindAllWithTag(const StringID tag)
	*
	* IT IS THE USERS RESPONSIBILITY TO FREE THE LIST AFTER THEY ARE DONE USING IT.
	*
	* Return Value: A dynamically allocated list containing GameObjectHandles corresponding to every GameObject with the requested tag.
	*/
	std::list<GameObjectHandle>* GameObject::FindAllWithTag(const StringID tag)
	{
		std::list<GameObjectHandle>* lst = new std::list<GameObjectHandle>();
		if (sTags[tag] != -1)
		{
			for (U64 i = 0; i < MAXNUMOBJECTS; ++i)
			{
				if (sGameObjectPool[i].mInitialized && sGameObjectPool[i].hasTag(tag))
				{
					lst->push_back(GameObjectHandle(&sGameObjectPool[i]));
				}
			}
		}
		return lst;
	}


   /*
	* GameObject::FindAll()
	*
	* IT IS THE USERS RESPONSIBILITY TO FREE THE LIST AFTER THEY ARE DONE USING IT.
	*
	* Return Value: return a list containing GameObjectHandles corresponding to every GameObject
	*/
	std::list<GameObjectHandle>* GameObject::FindAll()
	{ 
		std::list<GameObjectHandle>* goList = new std::list<GameObjectHandle>();
		for (U64 i = 0; i < MAXNUMOBJECTS; ++i)
		{
			if (sGameObjectPool[i].mInitialized)
			{
				goList->push_back(GameObjectHandle(&sGameObjectPool[i]));
			}
		}
		return goList;
	}


	static const StringID GameObjectID = hashCRC32("gameobject");
	static const StringID TagID = hashCRC32("tag");
	static const StringID NameID = hashCRC32("name");

   /*
	* isParent(boost::propert_tree::ptree goI)
	*
	* function fed to std::find, used to determine if the ptree representing the target ptrees parent is in the list.
	* 
	* Return Value: true  - if this ptree is the target ptrees parent.
	*				false - if this ptree is not the target ptrees parent.
	*/
	static std::string parentName;
	static bool isParent(boost::property_tree::ptree goI)
	{
		bool retVal = false;
		StringID goName;
		BOOST_FOREACH(boost::property_tree::ptree::value_type const & go, goI)
		{
			goName = lowerize(go.first.c_str());
			if (goName == NameID)
			{
				if (go.second.data().compare(parentName) == 0)
				{
					retVal = true;
				}
				break;
			}
		}
		return retVal;
	}


	static std::vector< std::list<boost::property_tree::ptree> > buckets;
	static bool ovr = false;

	static std::list<GameObjectHandle> enableList;


   /*
	* taskCreate(void* bucket)
	* 
	* The thread function used to create a GameObject from a ptree in one of the buckets for processing.
	*
	* Return Value: 0  - success
	*				-1 - failure
	*/
	int taskCreate(void* bucket)
	{
		using boost::property_tree::ptree;

		const U32 i = *((U32*)(bucket));
		ptree goI;
		
		Semaphore::Semaphore_wait(&bucketAccessSem);
		if (!buckets[i].empty())
		{
			goI = buckets[i].front();
			buckets[i].pop_front();
		}
		Semaphore::Semaphore_post(&bucketAccessSem);

		GameObjectHandle goh = GameObject::Create(goI, ovr);
		if (goh == GameObjectHandle::null)
		{
			return -1;
		}

		Semaphore::Semaphore_wait(&enableSem);
		enableList.push_back(goh);
		Semaphore::Semaphore_post(&enableSem);

		return 0;
	}


   /*
	* GameObject::LoadGameWorld(const char* gameWorldFile, bool overwrite = false)
	*
	* Load the GameWorld from the provided xml file.
	* if overwrite = true GameObjects with the same name will be overwritten by their counterparts read in by LoadGameWorld
	*/
	void GameObject::LoadGameWorld(const char* gameWorldFile, bool overwrite)
	{
		using boost::property_tree::ptree;
		ptree file;

		// Read the provided file.
		try
		{
			read_xml(gameWorldFile, file);
		}
		// Catch exception(invalid file)
		catch (boost::exception const&  ex)
		{
			gLogManager.log("Could not open file %s provided to LoadGameWorld(const char*)", gameWorldFile);
			static_cast<void>(ex);
			return;
		}
		catch (std::exception* e)
		{
			gLogManager.log("Could not open file %s provided to LoadGameWorld(const char*)", gameWorldFile);
			static_cast<void>(e);
			return;
		}

		boost::optional<ptree&> gameWorld = file.get_child_optional("GameWorld"); // This should be capital agnostic.
		if (!gameWorld)
		{
			gLogManager.log("Was no able to load a GameWorld from the file: %s", gameWorldFile);
		}
		else
		{
			buckets = std::vector< std::list<boost::property_tree::ptree> >(NUMBUCKETS + 1);
			std::list< std::list<ptree>::iterator > removalList;

			// Parse the GameWorld.
			StringID goName;
			BOOST_FOREACH(ptree::value_type const & go, *gameWorld)
			{
				goName = lowerize(go.first.c_str());

				// Get the subtrees for each of the GameObjects. 
				//	for later processing.
				if (goName == GameObjectID)
				{
					boost::optional<std::string> parent = go.second.get_optional<std::string>("parent");
					if (parent)
					{
						buckets[1].push_back(go.second);
					}
					else
					{
						buckets[0].push_back(go.second);
					}
				}
				// Register all global tags
				else if (goName == TagID)
				{
					gLogManager.log("Registering Tag %s...", go.second.data().c_str());
					bool b = GameObject::RegisterTag(internString(go.second.data().c_str()));
					gLogManager.log("	%s %s", (b ? "Registered tag:": "Failed to register tag:"), go.second.data().c_str());
				}
			}

			// Sort GameObject ptrees into buckets based on parents.
			gLogManager.log("Sorting GameObjects for creation...");
			for (U32 i = 1; i < NUMBUCKETS + 1; ++i)
			{
				// For each GameObject in bucket i.
				for (std::list<ptree>::iterator go = buckets[i].begin(); go != buckets[i].end(); ++go)
				{
					// Find its parent field.
					BOOST_FOREACH(ptree::value_type const & goField, *go)
					{
						// Account for capitalization preferences.
						StringID fieldID = lowerize(goField.first.c_str());
						if (fieldID == parentID)
						{
							parentName = goField.second.data();
							break;
						}
					}
					// Check to see if the parent is in bucket i too.
					std::list<ptree>::iterator parentPos = std::find_if(buckets[i].begin(), buckets[i].end(), isParent);

					// If it is.
					if (parentPos != buckets[i].end())
					{
						// This GameObject needs to be pushed back a bucket.
						buckets[i + 1].push_back(*go);
						removalList.push_back(go);
					}
				}

				// Remove all of the GameObjects that were moved to bucket i+1 from bucket i.
				for (std::list< std::list<ptree>::iterator >::iterator itd = removalList.begin(); itd != removalList.end();)
				{
					buckets[i].erase(*itd);
					++itd;
				}
				removalList.clear();
			}

			// Create actual GameObjects from the subtrees.
			ovr = overwrite;
			enableList.clear();
			// for each bucket. forward iteration because we are resolving parents.
			for (U32 i = 0; i < NUMBUCKETS + 1; ++i)
			{
				// Create n threads. Each thread creates one of the GameObjects in the bucket.
				std::vector<Thread> tVec(buckets[i].size());
				for (std::vector<Thread>::iterator t = tVec.begin(); t != tVec.end(); ++t)
				{
					Thread::Thread_create(&(*t), NULL, taskCreate, &i);
				}

				// Threads need to be waited on so that all GOs from previous buckets are created so parent/child relationships can be properly resolved.
				int status = 0;
				for (std::vector<Thread>::iterator t = tVec.begin(); t != tVec.end(); ++t)
				{
					Thread::Thread_waitFor(&(*t), &status);
				}
			}

			for (std::list<GameObjectHandle>::iterator goh = enableList.begin(); goh != enableList.end(); ++goh)
			{
				(*goh).enable();
			}
		}
	}


	void GameObject::LoadGameWorld1C(const char* gameWorldFile, bool overwrite)
	{
		using boost::property_tree::ptree;
		ptree file;

		// Read the provided file.
		try
		{
			read_xml(gameWorldFile, file);
		}
		// Catch exception(invalid file)
		catch (boost::exception const&  ex)
		{
			gLogManager.log("Could not open file %s provided to LoadGameWorld(const char*)", gameWorldFile);
			static_cast<void>(ex);
			return;
		}
		catch (std::exception* e)
		{
			gLogManager.log("Could not open file %s provided to LoadGameWorld(const char*)", gameWorldFile);
			static_cast<void>(e);
			return;
		}

		boost::optional<ptree&> gameWorld = file.get_child_optional("GameWorld"); // This should be capital agnostic.
		if (!gameWorld)
		{
			gLogManager.log("Was no able to load a GameWorld from the file: %s", gameWorldFile);
		}
		else
		{
			buckets = std::vector< std::list<boost::property_tree::ptree> >(NUMBUCKETS + 1);
			std::list< std::list<ptree>::iterator > removalList;

			// Parse the GameWorld.
			StringID goName;
			BOOST_FOREACH(ptree::value_type const & go, *gameWorld)
			{
				goName = lowerize(go.first.c_str());

				// Get the subtrees for each of the GameObjects. 
				//	for later processing.
				if (goName == GameObjectID)
				{
					boost::optional<std::string> parent = go.second.get_optional<std::string>("parent");
					if (parent)
					{
						buckets[1].push_back(go.second);
					}
					else
					{
						buckets[0].push_back(go.second);
					}
				}
				// Register all global tags
				else if (goName == TagID)
				{
					gLogManager.log("Registering Tag %s...", go.second.data().c_str());
					bool b = GameObject::RegisterTag(internString(go.second.data().c_str()));
					gLogManager.log("	%s %s", (b ? "Registered tag:" : "Failed to register tag:"), go.second.data().c_str());
				}
			}

			// Sort GameObject ptrees into buckets based on parents.
			gLogManager.log("Sorting GameObjects for creation...");
			for (U32 i = 1; i < NUMBUCKETS + 1; ++i)
			{
				// For each GameObject in bucket i.
				for (std::list<ptree>::iterator go = buckets[i].begin(); go != buckets[i].end(); ++go)
				{
					// Find its parent field.
					BOOST_FOREACH(ptree::value_type const & goField, *go)
					{
						// Account for capitalization preferences.
						StringID fieldID = lowerize(goField.first.c_str());
						if (fieldID == parentID)
						{
							parentName = goField.second.data();
							break;
						}
					}
					// Check to see if the parent is in bucket i too.
					std::list<ptree>::iterator parentPos = std::find_if(buckets[i].begin(), buckets[i].end(), isParent);

					// If it is.
					if (parentPos != buckets[i].end())
					{
						// This GameObject needs to be pushed back a bucket.
						buckets[i + 1].push_back(*go);
						removalList.push_back(go);
					}
				}

				// Remove all of the GameObjects that were moved to bucket i+1 from bucket i.
				for (std::list< std::list<ptree>::iterator >::iterator itd = removalList.begin(); itd != removalList.end();)
				{
					buckets[i].erase(*itd);
					++itd;
				}
				removalList.clear();
			}

			// Create actual GameObjects from the subtrees.
			gLogManager.log("Creating found GameObjects...");
			
			ovr = overwrite;
			enableList.clear();
			// for each bucket. forward iteration because we are resolving parents.
			for (U32 i = 0; i < NUMBUCKETS + 1; ++i)
			{
				for (std::list<boost::property_tree::ptree>::iterator goI = buckets[i].begin(); goI != buckets[i].end(); ++goI)
				{
					GameObjectHandle goh = GameObject::Create(*goI, ovr);
					enableList.push_back(goh);
				}
			}

			for (std::list<GameObjectHandle>::iterator goh = enableList.begin(); goh != enableList.end(); ++goh)
			{
				(*goh).enable();
			}

		}
	}


   /*
	* GameObject::SaveGameWorld(const char* gameWorldFile)
	*
	* Save the current state of all GameObjects to the requested xml file.
	* This function is used to generate World Files that represent a chunk of the level.
	*/
	void GameObject::SaveGameWorld(const char* gameWorldFile)
	{
		using boost::property_tree::ptree;
		ptree gwFile;
		ptree gw;

		std::list<GameObjectHandle>* gos = FindAll();

		// Add all registered tags to the file.
		for (TagPool::const_iterator tag = sTags.cbegin(); tag != sTags.cend(); ++tag)
		{
			gw.add("tag", getString((*tag).first));
		}

		// Add all of the GameObjects to the file.
		for (std::list<GameObjectHandle>::iterator go = gos->begin(); go != gos->end(); ++go)
		{
			ptree* goI = (*go).getObject()->SerializeOut();
			gw.add_child("GameObject", *goI);
			delete goI;
		}

		gwFile.add_child("GameWorld", gw);
		boost::property_tree::xml_writer_settings<char> settings('\t', 1);
		write_xml(gameWorldFile, gwFile, std::locale(), settings);
	}


   /*
	* GameObject::SaveDynamicGameWorld(const char* gameWorldFile)
	*
	* Save the current state of all non static GameObjects to the requested xml file.
	* This function is used to generate save files, that can be loaded over an already loaded World file to recreate a game state.
	*/
	void GameObject::SaveDynamicGameWorld(const char* gameWorldFile)
	{
		using boost::property_tree::ptree;
		ptree gwFile;
		ptree gw;

		std::list<GameObjectHandle>* gos = FindAll();

		// Add all registered tags to the file.
		for (TagPool::const_iterator tag = sTags.cbegin(); tag != sTags.cend(); ++tag)
		{
			gw.add("tag", getString((*tag).first));
		}

		// Add all of the GameObjects to the file.
		for (std::list<GameObjectHandle>::iterator go = gos->begin(); go != gos->end(); ++go)
		{
			if (!(*go).getObject()->mStatic)
			{
				ptree* goI = (*go).getObject()->SerializeOut();
				gw.add_child("GameObject", *goI);
				delete goI;
			}
		}

		gwFile.add_child("GameWorld", gw);
		boost::property_tree::xml_writer_settings<char> settings('\t', 1);
		write_xml(gameWorldFile, gwFile, std::locale(), settings);
	}
	

   /*
	* GameObject::RegisterTag(const StringID tag)
	*
	* Attempt to register the desired tag to the list of tags GameObjects can use to tag themselves.
	* 
	* Return Value: true  - the registration succeeded.
	*				false - the registration failed, caused by registering more than MAXNUMTAGS.
	*/
	bool GameObject::RegisterTag(const StringID tag)
	{
		Mutex::Mutex_lock(&tagMutex);
		const bool retVal = sTags.registerTag(tag);
		Mutex::Mutex_unlock(&tagMutex);
		return retVal;
	}


   /*
	* GameObject::UnregisterTag(const StringID tag)
	*
	* Attempt to unregister the desired tag.
	* successfully unregistering a tag frees up a slot for an additional tag to be registered.
	* 
	* Return Value: true  - tag was a previously registered tag.
	*				false - tag was not previously registered.
	*/
	bool GameObject::UnregisterTag(const StringID tag)
	{
		Mutex::Mutex_lock(&tagMutex);
		const bool retVal = sTags.RemoveTag(tag);
		Mutex::Mutex_unlock(&tagMutex);
		return retVal;
	}


	void GameObject::rmvP(GameObjectHandle go)
	{
		// Sync parent and child transforms
		if (go.transform().valid() && go.getParent().transform().valid())
		{
			go.getParent().transform().removeChild(go.transform());
		}

		// If go has a parent.
		GameObject* p = go.getObject()->mParent.getObject();
		std::list<GameObjectHandle>::iterator childLoc = std::find(p->mChildren.begin(), p->mChildren.end(), go);
		if (childLoc != p->mChildren.end())
		{
			// Remove it from its parents list of children.
			p->mChildren.erase(childLoc);
		}

		GameObject* gop = go.getObject();
		gop->mParent = GameObjectHandle::null;
		gop->mBucket = 0;

		// Sync gos scripts
		for (std::list<LuaScriptHandle>::iterator script = gop->mScripts.begin(); script != gop->mScripts.end(); ++script)
		{
			LuaScriptHandle::AddToBucket(0, *script);
		}

		std::list<GameObjectHandle>* children = gop->getChildren();
		for (std::list<GameObjectHandle>::iterator child = children->begin(); child != children->end(); ++child)
		{
			ppBucket(*child, 0);
		}
		delete children;
	}


	void GameObject::addP(GameObjectHandle go, GameObjectHandle p)
	{
		go.getObject()->mParent = p;
		go.getObject()->mBucket = p.getObject()->mBucket + 1;

		p.getObject()->mChildren.push_back(go);

		// Sync parent and child transforms.
		if (go.transform().valid() && go.getParent().transform().valid())
		{
			go.getParent().transform().addChild(go.transform());
		}

		// Sync gos scripts.
		for (std::list<LuaScriptHandle>::iterator script = go.getObject()->mScripts.begin(); script != go.getObject()->mScripts.end(); ++script)
		{
			LuaScriptHandle::AddToBucket(go.getObject()->mBucket, *script);
		}

		std::list<GameObjectHandle>* children = go.getChildren();
		for (std::list<GameObjectHandle>::iterator child = children->begin(); child != children->end(); ++child)
		{
			ppBucket(*child, go.getObject()->mBucket);
		}
	}


	void GameObject::ppBucket(GameObjectHandle go, I32 v)
	{
		I32 i = ++v;
		go.getObject()->mBucket = i;

		// Sync gos scripts.
		for (std::list<LuaScriptHandle>::iterator script = go.getObject()->mScripts.begin(); script != go.getObject()->mScripts.end(); ++script)
		{
			LuaScriptHandle::AddToBucket(go.getObject()->mBucket, *script);
		}

		std::list<GameObjectHandle>* children = go.getObject()->getChildren();
		for (std::list<GameObjectHandle>::iterator child = children->begin(); child != children->end(); ++child)
		{
			ppBucket(*child, i);
		}
		delete children;
	}


	StringID GameObject::NAME = hashCRC32("gameobject");
	U32 GameObject::MAXNUMOBJECTS;
	U32 GameObject::NUMBUCKETS;

	TagPool GameObject::sTags(MAXNUMTAGS);

	GameObject* GameObject::sFirstFree = NULL;
	GameObject* GameObject::sGameObjectPool = NULL;

	boost::unordered_map<StringID, GameObjectHandle> GameObject::sNameMap;
}
