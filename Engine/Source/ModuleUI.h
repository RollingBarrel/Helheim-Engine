#pragma once
#include "Module.h"
#include "Geometry/Frustum.h"

class GameObject;
class CanvasComponent;

class ENGINE_API ModuleUI : public Module
{
public:
	ModuleUI();
	~ModuleUI();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void CheckRaycast();
	void FindCanvas(GameObject* gameObject);

private:
	void DrawWidget(GameObject* gameObject);
	void CheckRaycastRecursive(GameObject* gameObject, bool& eventTriggered);
};

