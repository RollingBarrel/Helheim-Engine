#pragma once
#include "Module.h"
class GameObject;
class ModuleRenderTest :
    public Module
{
public:
	ModuleRenderTest();
	~ModuleRenderTest();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	const void DrawMeshRenderersRecursive(const GameObject* object);
	
};

