#pragma once
#include "Module.h"
#include "GameObject.h"
#include <vector>
class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();


	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

private:
	void GenerateRenderList(GameObject* root);
	void DrawRenderList();
	void AddToRenderList(GameObject* root); // Can be public if needed 

	std::vector<GameObject*> mRenderList;
};

