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
	void DrawEditor();
	void DrawInspector();
	void DrawHierarchy();

	GameObject* GetRoot() { return mRoot; }
	GameObject* GetSelectedGameObject() { return mSelectedGameObject; }

	void SetSelectedObject(GameObject* gameObject);
	void AddGameObjectToDelete(GameObject* gameObject) {
		mGameObjectsToDelete.push_back(gameObject);
	}

	void AddGameObjectToDuplicate(GameObject* gameObject) {
		mGameObjectsToDuplicate.push_back(gameObject);
	}

private:
	GameObject* mRoot;
	GameObject* mSelectedGameObject;
	std::vector<GameObject*> mGameObjectsToDelete;
	std::vector<GameObject*> mGameObjectsToDuplicate;
	void DeleteGameObjects();
	void DuplicateGameObjects();
};

