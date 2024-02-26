#pragma once
#include "Module.h"

class GameObject;

class ModuleUI :
    public Module
{
public:
	ModuleUI();
	~ModuleUI();

	bool Init();
	update_status Update();
	update_status PostUpdate();

private:
	void DrawWidget(const GameObject* gameObject);
};

