#pragma once
#include "Module.h"

class GameObject;

class ModuleScene : public Module
{
public:

	ModuleScene();
	~ModuleScene();
	bool Init() override;
	update_status Update() override;
	update_status PostUpdate() override;
	void DrawInspector();
	void DrawHierarchy();
	GameObject* getRoot() { return mRoot; }
	void SetSelectedObject(GameObject* gameObject);

private:
	GameObject* mRoot;
	GameObject* mSelectedGameObject;
};

