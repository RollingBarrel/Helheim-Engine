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
	inline unsigned int GetProgram() const { return mUIProgramId; }

	//***************************************************************
	void SetScreenSpace(bool screen);
	//***************************************************************

private:
	void DrawWidget(const GameObject* gameObject);

	unsigned int mQuadVBO;
	unsigned int mQuadVAO;
	GameObject* mCanvas = nullptr;

	unsigned int mUIProgramId = 0;

	unsigned int CreateShaderProgramFromIDs(unsigned vertexShaderID, unsigned fragmentShaderID) const;
	unsigned int CreateShaderProgramFromPaths(const char* vertexShaderPath, const char* fragmentShaderPath) const;
	char* LoadShaderSource(const char* shaderFileName) const;
	unsigned int CompileShader(unsigned type, const char* source) const;

	//*******************************************************
	bool mScreenSpace = true;
	//*******************************************************
};

