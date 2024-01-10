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


Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules[0] = window = new ModuleWindow();
	modules[1] = render = new ModuleOpenGL();
	modules[2] = fileSystem = new ModuleFileSystem();
	modules[3] = input = new ModuleInput();
	modules[4] = scene = new ModuleScene();
	modules[5] = test = new ModuleRenderTest();
	modules[6] = editor = new ModuleEditor();
	modules[7] = camera = new ModuleCamera();
	modules[8] = debugDraw = new ModuleDebugDraw();
}

Application::~Application()
{
	for(int i = 0; i < NUM_MODULES; ++i)
        delete modules[i];
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
	dt = timer.Read();
	timer.Start();
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
