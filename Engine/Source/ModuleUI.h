#pragma once
#include "Module.h"

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

	inline unsigned int GetQuadVAO() const { return mQuadVAO; }

private:
	void DrawWidget(const GameObject* gameObject);

	unsigned int mQuadVBO;
	unsigned int mQuadVAO;
	GameObject* mCanvas = nullptr;
};

