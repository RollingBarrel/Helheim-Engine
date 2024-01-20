#ifndef _MODULE_RENDER_TEST_H_
#define _MODULE_RENDER_TEST_H_

#include "Module.h"
struct ResourceMesh;

class ModuleRenderTest :
    public Module
{
public:
	ModuleRenderTest();
	~ModuleRenderTest();

	bool Init()override;
	update_status Update() override;
	bool CleanUp() override;

	unsigned int GetProgramId() const { return programId; }
	unsigned int GetDifuseTexture() const { return textureId; }
	unsigned int GetNormalTexture() const { return normTextureId; }
private:
	unsigned int VAO;
	unsigned int VBOEBO[2];
	unsigned int programId;
	unsigned int textureId;
	unsigned int normTextureId;

	float brightness = 20;
	float kD = 0.2;
	float lightDir[3] = { 0.0f, -1.0f, -1.0f };
	float lightCol[3] = { 1.f, 1.f, 1.f };
	float ambientCol[3] = { 0.3f, 0.4f, 0.6f };
	ResourceMesh* rMesh = nullptr;
	ResourceMesh* rMesh2 = nullptr;
};

#endif /* _MODULE_RENDER_TEST_H_ */
