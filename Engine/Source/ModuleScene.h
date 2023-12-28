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
	void AddGameObjectToDelete(const unsigned int id) {
		mGameObjectsToDelete.push_back(id);
	}

private:
	GameObject* mRoot;
	GameObject* mSelectedGameObject;
	std::vector<unsigned int> mGameObjectsToDelete;
	void DeleteGameObject(GameObject* gameObject);
};

