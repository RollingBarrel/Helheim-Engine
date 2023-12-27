#pragma once
#include "Module.h"
class ModuleRenderTest :
    public Module
{
public:
	ModuleRenderTest();
	~ModuleRenderTest();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;

	unsigned CreateProgram(unsigned vtx_shader, unsigned frg_shader);
	unsigned CompileShader(unsigned type, const char* source);
	char* LoadShaderSource(const char* shader_file_name);
	void RenderVBO(unsigned vbo, unsigned program);
	unsigned CreateTriangleVBO();
	void DestroyTriangleVBO();
	void RenderTriangleTest();

	
private:
	unsigned triangleVBO;
	unsigned fragment_id;
	unsigned vertex_id;
	unsigned program_id;
};

