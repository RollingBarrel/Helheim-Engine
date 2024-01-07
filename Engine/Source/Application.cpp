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
	mModules.push_back(window = new ModuleWindow());
	mModules.push_back(render = new ModuleOpenGL());
	mModules.push_back(input = new ModuleInput());
	mModules.push_back(scene = new ModuleScene());
	mModules.push_back(test = new ModuleRenderTest());
	mModules.push_back(editor = new ModuleEditor());
	mModules.push_back(camera = new ModuleCamera());
	mModules.push_back(debugDraw = new ModuleDebugDraw());

}

Application::~Application()
{
	for(std::list<Module*>::iterator it = mModules.begin(); it != mModules.end(); ++it)
    {
        delete *it;
    }
}

bool Application::Init()
{
	bool ret = true;

	for(std::list<Module*>::iterator it = mModules.begin(); it != mModules.end() && ret; ++it)
		ret = (*it)->Init();

	return ret;
}

update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;

	for(std::list<Module*>::iterator it = mModules.begin(); it != mModules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PreUpdate();

	for(std::list<Module*>::iterator it = mModules.begin(); it != mModules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->Update();

	for(std::list<Module*>::iterator it = mModules.begin(); it != mModules.end() && ret == UPDATE_CONTINUE; ++it)
		ret = (*it)->PostUpdate();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for(std::list<Module*>::reverse_iterator it = mModules.rbegin(); it != mModules.rend() && ret; ++it)
		ret = (*it)->CleanUp();

	return ret;
}
