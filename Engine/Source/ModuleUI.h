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

	void LoadVBO();
	void CreateVAO();

	inline unsigned int GetQuadVAO() const { return mQuadVAO; }

private:
	void DrawWidget(const GameObject* gameObject);

	unsigned int mQuadVBO;
	unsigned int mQuadVAO;
};

