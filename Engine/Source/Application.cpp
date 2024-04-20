#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleInput.h"
#include "ModuleEditor.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleFileSystem.h"
#include "ModuleScriptManager.h"
#include "ModuleResource.h"
#include "ModuleDetourNavigation.h"
#include "ModuleUI.h"
#include "ModuleAudio.h"
#include "ModuleEvent.h"

#include "Timer.h"
#include "PreciseTimer.h"


Application::Application()
{
	mEngineTimer = new Timer();
	mGameTimer = new Timer();

	//In case we want to use precise timer
	
	//mEngineTimer = new PreciseTimer();
	//mGameTimer = new PreciseTimer();

	// Order matters: they will Init/start/update in this order
	modules[0] = input = new ModuleInput();
	modules[1] = window = new ModuleWindow();
	modules[2] = camera = new ModuleCamera();
	modules[3] = fileSystem = new ModuleFileSystem();
	modules[4] = audio = new ModuleAudio();
	modules[5] = render = new ModuleOpenGL();
	modules[6] = resource = new ModuleResource();
	modules[7] = scriptManager = new ModuleScriptManager();
	modules[8] = debugDraw = new ModuleDebugDraw();
	modules[9] = scene = new ModuleScene();
	modules[10] = navigation = new ModuleDetourNavigation();
	modules[11] = editor = new ModuleEditor();
	modules[12] = ui = new ModuleUI();
	modules[13] = event = new ModuleEvent();
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
	mCurrentTimer->Update();		//Updates the current timer variables (Engine or Game depending on the game state)

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
	editor->SaveCameraPosition();
	bool ret = true;

	for (int i = 0; i < NUM_MODULES; ++i)
		ret = modules[i]->CleanUp();
	
	return ret;
}

float Application::GetRealDt() const {
	return mEngineTimer->GetDelta() / (float) 1000;
}

float Application::GetGameDt() const {
	return mGameTimer->GetDelta() / (float)1000;
}