#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleRenderTest.h"
#include "ModuleDebugDraw.h"
#include "ModuleFileSystem.h"
#include "ModuleTimer.h"
#include "ModuleSkybox.h"

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules[0] = clock = new ModuleTimer();
	modules[1] = input = new ModuleInput();
	modules[2] = window = new ModuleWindow();
	modules[3] = render = new ModuleOpenGL();
	modules[4] = camera = new ModuleCamera();
	modules[5] = fileSystem = new ModuleFileSystem();
	modules[6] = debugDraw = new ModuleDebugDraw();
	modules[7] = scene = new ModuleScene();
	modules[8] = skybox = new ModuleSkybox();
	modules[9] = test = new ModuleRenderTest();
	modules[10] = editor = new ModuleEditor();

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