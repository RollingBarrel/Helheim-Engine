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
	NavMeshController* GetNavController() const { return mNavMeshController; }

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
	void SetShouldRenderQuadtree(bool drawQuadtree) { mDrawQuadtree = drawQuadtree; }
	bool GetShouldUpdateQuadtree() const { return mShouldUpdateQuadtree; }
	void SetShouldUpdateQuadtree(bool updateQuadtree) { mShouldUpdateQuadtree = updateQuadtree; }

	bool GetApplyFrustumCulling() const { return mApplyculling; }
	void SetApplyFrustumCulling(bool applyFrustumCulling) { mApplyculling = applyFrustumCulling; }
	void ResetFrustumCulling(GameObject* obj);

	GameObject* FindGameObjectWithTag(GameObject* root, unsigned tagID);
	GameObject* FindGameObjectWithTag(unsigned tagID);
	GameObject* FindGameObjectWithTag(const char* tagName);
	void FindGameObjectsWithTag(GameObject* root, unsigned tagid, std::vector<GameObject*>& foundGameObjects);

	void AddTag(std::string tag);
	unsigned int GetSize() { return static_cast<unsigned int>(mTags.size()); };
	int GetCustomTagsSize();
	std::vector<Tag*> GetAllTags() { return mTags; };
	std::vector<Tag*> GetSystemTag();
	std::vector<Tag*> GetCustomTag();
	Tag* GetTagByName(std::string tagname);
	Tag* GetTagByID(unsigned id);
	void DeleteTag(Tag* tag);

	void Save(const char* saveFilePath) const;
	void Load(const char* saveFilePath);
	int SavePrefab(const GameObject* gameObject, const char* saveFilePath) const;
	void LoadPrefab(const char* saveFilePath, unsigned int resourceId, bool update = false);
	void OpenPrefabScreen(const char* saveFilePath);
	void ClosePrefabScreen();
	bool IsPrefabScene() const { return mBackgroundScene != nullptr; }

	GameObject* Find(const char* name) const;
	GameObject* Find(unsigned int UID) const;

private:
	void DeleteGameObjects();
	void DuplicateGameObjects();
	void LoadGameObjectsIntoScripts();

	void SaveGame(const std::vector<GameObject*>& gameObjects, Archive& rootArchive) const;
	void SaveGameObjectRecursive(const GameObject* gameObject, std::vector<Archive>& gameObjectsArchive, int parentUuid) const;

	Quadtree* mQuadtreeRoot = nullptr;;
	bool mShouldUpdateQuadtree = false;
	bool mDrawQuadtree = false;
	bool mApplyculling = false;

	GameObject* mRoot = nullptr;
	GameObject* mBackgroundScene = nullptr;
	const char* mPrefabPath = "";
	bool mClosePrefab = false;
	NavMeshController* mNavMeshController;

	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
	std::vector<std::pair<unsigned int, GameObject**>> mGameObjectsToLoadIntoScripts;


	std::vector<Tag*> mTags;

	unsigned mLastTagIndex = 10;

};

#endif //_MODULE_SCENE_H_