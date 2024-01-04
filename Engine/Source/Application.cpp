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


Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(render = new ModuleOpenGL());
	modules.push_back(input = new ModuleInput());
	modules.push_back(scene = new ModuleScene());
	modules.push_back(test = new ModuleRenderTest());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(camera = new ModuleCamera());
	modules.push_back(debugDraw = new ModuleDebugDraw());

}

Application::~Application()
{
	for(std::list<Module*>::iterator it = modules.begin(); it != modules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret; ++it)
		ret = (*it)->Init();

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	for(std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(std::list<Module*>::iterator it = modules.begin(); it != modules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(std::list<Module*>::reverse_iterator it = modules.rbegin(); it != modules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
