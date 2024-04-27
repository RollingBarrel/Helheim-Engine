#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleFileSystem.h"
#include "ModuleScriptManager.h"
#include "ModuleResource.h"
#include "ModuleDetourNavigation.h"
#include "ModuleUI.h"
#include "ModuleAudio.h"
#include "ModuleEvent.h"

#include "Timer.h"
#include "PreciseTimer.h"

#ifdef ENGINE
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "optick.h"
#else
#include "ModuleGame.h"
#endif // ENGINE


#include "Main.h"


Application::Application()
{

	//In case we want to use precise timer
	
	//mEngineTimer = new PreciseTimer();
	//mGameTimer = new PreciseTimer();

	// Order matters: they will Init/start/update in this order
#ifdef ENGINE
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
#else
	modules[0] = input = new ModuleInput();
	modules[1] = window = new ModuleWindow();
	modules[2] = camera = new ModuleCamera();
	modules[3] = fileSystem = new ModuleFileSystem();
	modules[4] = audio = new ModuleAudio();
	modules[5] = render = new ModuleOpenGL();
	modules[6] = resource = new ModuleResource();
	modules[7] = scriptManager = new ModuleScriptManager();
	modules[8] = scene = new ModuleScene();
	modules[9] = navigation = new ModuleDetourNavigation();
	modules[10] = ui = new ModuleUI();
	modules[11] = event = new ModuleEvent();
	modules[12] = game = new ModuleGame();
#endif // ENGINE

}

Application::~Application()
{
	for (int i = NUM_MODULES-1; i >= 0; --i) {
		delete modules[i];
	}
	if (mEngineTimer)
		delete mEngineTimer;
	delete mGameTimer;
}

bool Application::Init()
{
#ifdef ENGINE
	mEngineTimer = new Timer();
	mGameTimer = new Timer();
	mCurrentTimer = mEngineTimer;
	mEngineTimer->Start();
#else
	mGameTimer = new Timer();
	mCurrentTimer = mGameTimer;
	mGameTimer->Start();
	mIsPlayMode = true;
#endif // ENGINE


	bool ret = true;

	for(int i = 0; i < NUM_MODULES && ret == true; ++i)
		ret = modules[i]->Init();

	//mRealTimer->StartWithRunTime();

	return ret;
}

update_status Application::Update(float dt)
{
#ifdef ENGINE
	OPTICK_FRAME("MainThread");
#endif // ENGINE
	
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
#ifdef ENGINE
	editor->SaveCameraPosition();
#endif // ENGINE
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