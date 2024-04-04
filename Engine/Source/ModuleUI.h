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

	inline unsigned int GetQuadVAO() const { return mQuadVAO; }
	inline unsigned int GetProgram() const { return mUIProgramId; }
	inline Frustum* GetFrustum() const { return mCurrentFrustum; }

	void SetScreenSpace(bool screen) { mScreenSpace = screen; }
	inline bool GetScreenSpace() { return mScreenSpace; }
	
	void CheckRaycast();
	void ResizeFrustum(unsigned int width, unsigned int height);

private:
	void DrawWidget(GameObject* gameObject);
	GameObject* FindCanvas(GameObject* gameObject);

	Frustum* mUIfrustum = nullptr;
	Frustum* mCurrentFrustum = nullptr; 

	unsigned int mQuadVBO;
	unsigned int mQuadVAO;
	GameObject* mCanvas = nullptr;

	unsigned int mUIProgramId = 0;

	unsigned int CreateShaderProgramFromIDs(unsigned vertexShaderID, unsigned fragmentShaderID) const;
	unsigned int CreateShaderProgramFromPaths(const char* vertexShaderPath, const char* fragmentShaderPath) const;
	char* LoadShaderSource(const char* shaderFileName) const;
	unsigned int CompileShader(unsigned type, const char* source) const;

	bool mScreenSpace = false;
};

