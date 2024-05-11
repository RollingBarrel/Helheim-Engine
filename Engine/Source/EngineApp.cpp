#include "EngineApp.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleEngineInput.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleFileSystem.h"
#include "ModuleEngineScriptManager.h"
#include "ModuleResource.h"
#include "ModuleDetourNavigation.h"
#include "ModuleEngineResource.h"
#include "ModuleUI.h"
#include "ModuleAudio.h"
#include "ModuleEvent.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "Timer.h"
#include "NavMeshController.h"

EngineApplication* EngineApp = NULL;

EngineApplication::EngineApplication()
{
	EngineApp = this;

	mEngineTimer = new Timer();
	mGameTimer = new Timer();
	
	modules[0] = engineInput = new ModuleEngineInput();
	input = engineInput;
	modules[1] = window = new ModuleWindow();
	modules[2] = camera = new ModuleCamera();
	modules[3] = fileSystem = new ModuleFileSystem();
	modules[4] = audio = new ModuleAudio();
	modules[5] = render = new ModuleOpenGL();
	modules[6] = engineResources = new ModuleEngineResource();
	resource = engineResources;
	modules[7] = engineScriptManager = new ModuleEngineScriptManager();
	scriptManager = engineScriptManager;
	modules[8] = debugDraw = new ModuleDebugDraw();
	modules[9] = scene = new ModuleScene();
	modules[10] = navigation = new ModuleDetourNavigation();
	modules[11] = editor = new ModuleEditor();
	modules[12] = ui = new ModuleUI();
	modules[13] = event = new ModuleEvent();
	mNavMeshController = new NavMeshController();
}

EngineApplication::~EngineApplication()
{
	for (int i = NUM_MODULES - 1; i >= 0; --i) 
	{
		delete modules[i];
	}
	delete mEngineTimer;
	delete mGameTimer;
	delete mNavMeshController;
}

bool EngineApplication::Init()
{
	mEngineTimer = new Timer();
	mGameTimer = new Timer();
	mCurrentTimer = mEngineTimer;
	mEngineTimer->Start();
	//#else
	//	mGameTimer = new Timer();
	//	mCurrentTimer = mGameTimer;
	//	mGameTimer->Start();
	//	mIsPlayMode = true;
	//#endif // ENGINE

	bool ret = true;

	for (int i = 0; i < NUM_MODULES && ret == true; ++i)
		ret = modules[i]->Init();

	return ret;
}

update_status EngineApplication::Update(float dt)
{
	//OPTICK_FRAME("MainThread");

	mCurrentTimer->Update();

	update_status ret = UPDATE_CONTINUE;

	for (int i = 0; i < NUM_MODULES && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->PreUpdate(dt);

	for (int i = 0; i < NUM_MODULES && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->Update(dt);

	for (int i = 0; i < NUM_MODULES && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->PostUpdate(dt);

	return ret;
}

bool EngineApplication::CleanUp()
{
	editor->SaveCameraPosition();
	bool ret = true;

	for (int i = 0; i < NUM_MODULES; ++i)
		ret = modules[i]->CleanUp();

	return ret;
}

float EngineApplication::GetRealDt() const
{
	return mEngineTimer->GetDelta() / (float)1000;
}

float EngineApplication::GetGameDt() const
{
	return mGameTimer->GetDelta() / (float)1000;
}
