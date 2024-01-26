#ifndef _MODULE_OPENGL_H_
#define _MODULE_OPENGL_H_

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
	update_status PostUpdate() override;
	bool CleanUp() override;

	void WindowResized(unsigned width, unsigned height);
	void SceneFramebufferResized(unsigned width, unsigned height);
	unsigned int GetFramebufferTexture() const { return colorAttachment; }
	void BindSceneFramebuffer();
	void UnbindSceneFramebuffer();
	void SetOpenGlCameraUniforms() const;
	void* GetOpenGlContext() { return context; }

	unsigned int GetPBRProgramId() const { return mPbrProgramId; }
	unsigned int GetSkyboxProgramId() const { return mSkyBoxProgramId; }

private:
	void* context = nullptr;

	//Framebuffer
	unsigned int sFbo;
	unsigned int colorAttachment;
	unsigned int depthStencil;

	//Camera
	unsigned int cameraUnis;

	//Skybox
	void InitSkybox();
	unsigned int mSkyBoxTexture = 0;
	unsigned int mSkyVao = 0;
	unsigned int mSkyVbo = 0;

	//Programs
	char* LoadShaderSource(const char* shaderFileName) const;
	unsigned CompileShader(unsigned type, const char* source) const;
	unsigned CreateShaderProgramFromIDs(unsigned vertexShaderID, unsigned fragmentShaderID) const;
	unsigned CreateShaderProgramFromPaths(const char* vertexShaderPath, const char* fragmentShaderPath) const;
	unsigned int mPbrProgramId = 0;
	unsigned int mSkyBoxProgramId = 0;


	//Lighting uniforms
	float mLightIntensity = 1.2f;
	float mLightDir[3] = { 0.0f, -1.0f, -1.0f };
	float mLightCol[3] = { 1.f, 1.f, 1.f };
	float mAmbientCol[3] = { 0.3f, 0.4f, 0.6f };
	friend class LightningPanel;
};

#endif /* _MODULEOPENGL_H_ */