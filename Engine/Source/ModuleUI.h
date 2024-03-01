#pragma once
#include "Module.h"

class GameObject;
class CanvasComponent;

class ModuleUI : public Module
{
public:
	ModuleUI();
	~ModuleUI();

<<<<<<< Updated upstream
	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status Update(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;
=======
	bool Init();
	update_status Update(float dt);
	update_status PostUpdate();
>>>>>>> Stashed changes

	void LoadVBO();
	void CreateVAO();

	inline unsigned int GetQuadVAO() const { return mQuadVAO; }

private:
	void DrawWidget(const GameObject* gameObject);

	unsigned int mQuadVBO;
	unsigned int mQuadVAO;
	GameObject* mCanvas = nullptr;
};

