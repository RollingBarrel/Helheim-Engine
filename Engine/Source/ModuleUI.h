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

	void DrawWidget(GameObject* gameObject);
	void DrawBlurWidget(GameObject* gameObject);
	
	void RemoveCanvas(GameObject* gameObject);
	void AddCanvas(GameObject* gameObject);

private:
	void UiBlurPass();
	unsigned int mBlurIntensity = 5;

	std::vector<GameObject*> mCanvasList;
};

