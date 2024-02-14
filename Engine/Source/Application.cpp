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

#include "Timer.h"
#include "PreciseTimer.h"


Application::Application()
{
	mEngineTimer = new Timer();
	mGameTimer = new Timer();

	// Order matters: they will Init/start/update in this order
	modules[0] = input = new ModuleInput();
	modules[1] = window = new ModuleWindow();
	modules[2] = camera = new ModuleCamera();
	modules[3] = fileSystem = new ModuleFileSystem();
	modules[4] = render = new ModuleOpenGL();
	modules[5] = debugDraw = new ModuleDebugDraw();
	modules[6] = scene = new ModuleScene();
	modules[7] = editor = new ModuleEditor();

}

Application::~Application()
{
	for (int i = NUM_MODULES-1; i >= 0; --i) {
		delete modules[i];
	}

	delete mEngineTimer;
	delete mGameTimer;
}

bool Application::Init()
{
	mEngineTimer->Start();			//Initializes the Engine timer
	mCurrentTimer = mEngineTimer;	//The application starts in the editor

	bool ret = true;

	for(int i = 0; i < NUM_MODULES && ret == true; ++i)
		ret = modules[i]->Init();

	//mRealTimer->StartWithRunTime();

	return ret;
}

update_status Application::Update(float dt)
{
	mCurrentTimer->Update();		//Updates the current timer (Engine or Game depending on the game state)

	update_status ret = UPDATE_CONTINUE;

	for (int i = 0; i < NUM_MODULES && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->PreUpdate(dt);

	for (int i = 0; i < NUM_MODULES && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->Update(dt);

	for (int i = 0; i < NUM_MODULES && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->PostUpdate(dt);

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
	return mEngineTimer->GetDelta() / (float) 1000;
}