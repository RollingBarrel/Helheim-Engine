#ifndef _MODULE_SCENE_H_
#define _MODULE_SCENE_H_

#include "Module.h"
#include <vector>
#include <string>
class Quadtree;
class GameObject;
class MeshRendererComponent;
class Archive;
class Tag;
class NavMeshController;

class ENGINE_API ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();
	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;

	GameObject* GetRoot() const { return mRoot; }
<<<<<<< HEAD
<<<<<<< HEAD
	NavMeshController* GetNavController() const { return mNavMeshController; }
=======
	GameObject* GetCanvas() const { return mCanvas; }
>>>>>>> a5c51ef (Module UI initial structure)
=======
>>>>>>> ad6ffa9 (Canvas Component)

	void AddGameObjectToDelete(GameObject* gameObject) {
		mGameObjectsToDelete.push_back(gameObject);
	}

	void AddGameObjectToDuplicate(GameObject* gameObject) {
		mGameObjectsToDuplicate.push_back(gameObject);
	}

	void AddGameObjectToLoadIntoScripts(std::pair<unsigned int, GameObject**> pair) {
		mGameObjectsToLoadIntoScripts.push_back(pair);
	}

	Quadtree* GetQuadtreeRoot() const { return mQuadtreeRoot; }
	bool GetShouldRenderQuadtree() const { return mDrawQuadtree; }
	void SetShouldRenderQuadtree(bool a) { mDrawQuadtree = a; }

	bool GetApplyFrustumCulling() const { return mApplyculling; }
	void SetApplyFrustumCulling(bool a) { mApplyculling = a; }

	GameObject* FindGameObjectWithTag(GameObject* root, unsigned tagid);
	void FindGameObjectsWithTag(GameObject* root, unsigned tagid, std::vector<GameObject*>& foundGameObjects);

	void AddTag(std::string tag);
	int GetSize() { return mTags.size(); };
	int GetCustomTagsSize();
	std::vector<Tag*> GetAllTags() { return mTags; };
	std::vector<Tag*> GetSystemTag();
	std::vector<Tag*> GetCustomTag();
	Tag* GetTagByName(std::string tagname);
	Tag* GetTagByID(unsigned id);
	void DeleteTag(Tag* tag);

	void Save(const char* saveFilePath);
	void Load(const char* saveFilePath);

	GameObject* Find(const char* name);
	GameObject* Find(unsigned int UID);

private:
	void DeleteGameObjects();
	void DuplicateGameObjects();
	void LoadGameObjectsIntoScripts();

	void SaveGameObjectRecursive(const GameObject* gameObject, std::vector<Archive>& gameObjectsArchive);
	void SaveGame(const std::vector<GameObject*>& gameObjects, Archive& rootArchive);
	
	Quadtree* mQuadtreeRoot;
	bool mDrawQuadtree = false;
	bool mApplyculling = false;

	GameObject* mRoot = nullptr;
<<<<<<< HEAD
<<<<<<< HEAD
	NavMeshController* mNavMeshController;
=======
	GameObject* mCanvas = nullptr;
>>>>>>> a5c51ef (Module UI initial structure)
=======
>>>>>>> ad6ffa9 (Canvas Component)

	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
	std::vector<std::pair<unsigned int, GameObject**>> mGameObjectsToLoadIntoScripts;


	std::vector<Tag*> mTags;

	unsigned mLastTagIndex = 10;

};

#endif //_MODULE_SCENE_H_
