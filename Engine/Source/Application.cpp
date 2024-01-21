#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleFileSystem.h"
#include "ModuleProgram.h"
#include "ModuleTimer.h"
#include "ModuleSkybox.h"


Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules[0] = input = new ModuleInput();
	modules[1] = window = new ModuleWindow();
	modules[2] = render = new ModuleOpenGL();
	modules[3] = program = new ModuleProgram("PBR_VertexShader.glsl", "PBR_PixelShader.glsl");
	modules[4] = camera = new ModuleCamera();
	modules[5] = fileSystem = new ModuleFileSystem();
	modules[6] = skybox = new ModuleSkybox();
	modules[7] = debugDraw = new ModuleDebugDraw();
	modules[8] = scene = new ModuleScene();
	modules[9] = editor = new ModuleEditor();
	modules[10] = clock = new ModuleTimer();
}

Application::~Application()
{
	for (int i = 0; i < NUM_MODULES; ++i) {
		delete modules[i];
	}
}

bool Application::Init()
{
	bool ret = true;

	for(int i = 0; i < NUM_MODULES && ret == true; ++i)
		ret = modules[i]->Init();

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	for (int i = 0; i < NUM_MODULES && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->PreUpdate();

	for (int i = 0; i < NUM_MODULES && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->Update();

	for (int i = 0; i < NUM_MODULES && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (int i = 0; i < NUM_MODULES; ++i)
		ret = modules[i]->CleanUp();
	
	return ret;
}

float Application::GetDt() const {
	return clock->GetRealDelta() / (float) 1000; 
}