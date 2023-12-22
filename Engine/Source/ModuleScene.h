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
	void DrawInspector();
	void DrawHierarchy();

private:
	GameObject* mRoot;

};

