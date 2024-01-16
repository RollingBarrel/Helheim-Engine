#pragma once
#include "Module.h"
#include <vector>

class GameObject;
class ModuleScene : public Module
{
public:
	ModuleScene();
	~ModuleScene();
	bool Init() override;
	update_status Update() override;
	update_status PostUpdate() override;

	GameObject* GetRoot() { return mRoot; }

	void AddGameObjectToDelete(GameObject* gameObject) {
		mGameObjectsToDelete.push_back(gameObject);
	}

	void AddGameObjectToDuplicate(GameObject* gameObject) {
		mGameObjectsToDuplicate.push_back(gameObject);
	}

private:
	void DeleteGameObjects();
	void DuplicateGameObjects();

	GameObject* mRoot = nullptr;
	GameObject* mSelectedGameObject = nullptr;
	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
	void DeleteGameObjects();
	void DuplicateGameObjects();
};

