#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleOpenGL : public Module
{
public:
	ModuleOpenGL();
	~ModuleOpenGL();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	void WindowResized(unsigned width, unsigned height);

public:
	void GenerateRenderList(GameObject* root);
	void DrawRenderList();
	void AddToRenderList(GameObject* root); // Can be public if needed 

	std::vector<GameObject*> mRenderList;
	void* context = nullptr;
};
