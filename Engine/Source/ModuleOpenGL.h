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
	void* GetOpenGlContext() { return context; }

private:

	void* context = nullptr;

	unsigned int sFbo;
	unsigned int colorAttachment;
	unsigned int depthStencil;

	//Lighting uniforms
	float mLightIntensity = 1.2f;
	float mLightDir[3] = { 0.0f, -1.0f, -1.0f };
	float mLightCol[3] = { 1.f, 1.f, 1.f };
	float mAmbientCol[3] = { 0.3f, 0.4f, 0.6f };
	friend class LightningPanel;
};

#endif /* _MODULEOPENGL_H_ */