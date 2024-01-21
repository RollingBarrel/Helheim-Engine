#pragma once
#include "Application.h"
#include "Module.h"
#include "DirectXTex.h"
#include "MathGeoLib.h"
class ModuleRenderSkybox :
	public Module
{
public:
	ModuleRenderSkybox();
	~ModuleRenderSkybox();

	bool Init();
	update_status PreUpdate();
	bool CleanUp();

	unsigned LoadCubeMap();
	unsigned int texture = 0;
	unsigned int vao = 0;
	unsigned int vbo = 0;
	unsigned int program = 0;

	float4x4 proj = float4x4::identity;;
	float4x4 model = float4x4::identity;
	float4x4 view = float4x4::identity;
};