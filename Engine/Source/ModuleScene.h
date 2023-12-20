#pragma once
#include "Module.h"

class GameObject;

class ModuleScene : public Module
{
public:

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	void DrawEditor();

private:
	GameObject* mRoot;


};

