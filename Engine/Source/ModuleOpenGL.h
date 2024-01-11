#ifndef _MODULE_OPENGL_H_
#define _MODULE_OPENGL_H_

#include "Module.h"
#include "Globals.h"

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
	unsigned int GetFramebufferTexture() const { return colorAttachment; }
	void BindFramebuffer();
	void UnbindFramebuffer();

public:
	void* context = nullptr;

	unsigned int fbo;
	unsigned int colorAttachment;
	unsigned int depthStencil;
};

#endif /* _MODULEOPENGL_H_ */