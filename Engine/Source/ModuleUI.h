#pragma once
#include "Module.h"
#include "Geometry/Frustum.h"

class GameObject;
class CanvasComponent;

class ModuleUI : public Module
{
public:
	ModuleUI();
	~ModuleUI();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void LoadVBO();
	void CreateVAO();

	void SetScreenSpace(bool screen) { mScreenSpace = screen; }
	inline bool GetScreenSpace() { return mScreenSpace; }
	
	void CheckRaycast();
	void FindCanvas(GameObject* gameObject);

private:
	void DrawWidget(GameObject* gameObject);

	GameObject* mCanvas = nullptr;

	bool mScreenSpace = true;
};

