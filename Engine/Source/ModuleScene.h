#pragma once
#include "Module.h"
#include <vector>

class Quadtree;
class GameObject;
class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();
	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	GameObject* GetRoot() const { return mRoot; }

	void AddGameObjectToDelete(GameObject* gameObject) {
		mGameObjectsToDelete.push_back(gameObject);
	}

	void AddGameObjectToDuplicate(GameObject* gameObject) {
		mGameObjectsToDuplicate.push_back(gameObject);
	}

	Quadtree* GetQuadtreeRoot() const { return mQuadtreeRoot; }
	bool* GetShouldRenderQuadtree() const { return mDrawQuadtree; }

private:
	void DeleteGameObjects();
	void DuplicateGameObjects();
	
	Quadtree* mQuadtreeRoot;
	bool* mDrawQuadtree = new bool(false);

	GameObject* mRoot = nullptr;

	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
};

