#ifndef _MODULE_SCENE_H_
#define _MODULE_SCENE_H_

#include "Module.h"
#include <vector>
class Quadtree;
class GameObject;
class Archive;

class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();
	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;

	GameObject* GetRoot() const { return mRoot; }

	void AddGameObjectToDelete(GameObject* gameObject) {
		mGameObjectsToDelete.push_back(gameObject);
	}

	void AddGameObjectToDuplicate(GameObject* gameObject) {
		mGameObjectsToDuplicate.push_back(gameObject);
	}

	Quadtree* GetQuadtreeRoot() const { return mQuadtreeRoot; }
	bool GetShouldRenderQuadtree() const { return mDrawQuadtree; }
	void SetShouldRenderQuadtree(bool a) { mDrawQuadtree = a; }

	bool GetApplyFrustumCulling() const { return mApplyculling; }
	void SetApplyFrustumCulling(bool a) { mApplyculling = a; }

	const std::vector<GameObject*> GetRenderList() { return mRenderList; }

	void Save(const char* saveFilePath);
	void Load(const char* saveFilePath);
	void SaveGameObjectRecursive(const GameObject* gameObject, std::vector<Archive>& gameObjectsArchive);

private:
	void DeleteGameObjects();
	void DuplicateGameObjects();
	void GenerateRenderList(GameObject* root);
	void DrawRenderList();
	void AddToRenderList(GameObject* root); // Can be public if needed 

	void SaveGame(const std::vector<GameObject*>& gameObjects, Archive& rootArchive);
	
	Quadtree* mQuadtreeRoot;
	bool mDrawQuadtree = false;
	bool mApplyculling = false;

	GameObject* mRoot = nullptr;

	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
	std::vector<GameObject*> mRenderList;

};

#endif //_MODULE_SCENE_H_
