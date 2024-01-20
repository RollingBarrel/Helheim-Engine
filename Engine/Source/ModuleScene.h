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
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	GameObject* GetRoot() { return mRoot; }
	GameObject* GetSelectedGameObject() { return mSelectedGameObject; }

	void SetSelectedObject(GameObject* gameObject);
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

private:
	void DeleteGameObjects();
	void DuplicateGameObjects();
	void SaveGameObjectRecursive(const GameObject& gameObject, Archive& gameObjectsArchive);
	void SaveGame(const std::vector<GameObject*>& gameObjects, Archive& rootArchive);
	
	Quadtree* mQuadtreeRoot;
	bool mDrawQuadtree = false;
	bool mApplyculling = false;

	GameObject* mRoot = nullptr;
	GameObject* mSelectedGameObject = nullptr;

	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
	
};

#endif //_MODULE_SCENE_H_
