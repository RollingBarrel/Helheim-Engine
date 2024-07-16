#include "EngineApp.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleEngineInput.h"
#include "ModuleScene.h"
#include "ModuleEngineCamera.h"
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
#include "ModulePhysics.h"
#include "Timer.h"
#include "optick.h"
#include "NavMeshController.h"

EngineApplication* EngineApp = NULL;

EngineApplication::EngineApplication()
{
	EngineApp = this;

	mIsPlayMode = false;

	mEngineTimer = new Timer();
	mGameTimer = new Timer();
	
	modules[0] = engineInput = new ModuleEngineInput();
	input = engineInput;
	modules[1] = window = new ModuleWindow();
	modules[2] = engineCamera = new ModuleEngineCamera();
	camera = engineCamera;
	modules[3] = fileSystem = new ModuleFileSystem();
	modules[4] = audio = new ModuleAudio();
	modules[5] = render = new ModuleOpenGL();
	modules[6] = engineResources = new ModuleEngineResource();
	resource = engineResources;
	modules[7] = physics = new ModulePhysics();
	modules[8] = engineScriptManager = new ModuleEngineScriptManager();
	scriptManager = engineScriptManager;
	modules[9] = scene = new ModuleScene();
	modules[10] = debugDraw = new ModuleDebugDraw();
	modules[11] = navigation = new ModuleDetourNavigation();
	modules[12] = ui = new ModuleUI();
	modules[13] = editor = new ModuleEditor();
	modules[14] = event = new ModuleEvent();
	
	mNavMeshController = new NavMeshController();
}

EngineApplication::~EngineApplication()
{
	for (int i = NUM_MODULES - 1; i >= 0; --i) 
	{
		delete modules[i];
		modules[i] = nullptr;
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
	OPTICK_FRAME("EngineThread");
	if (mExit) return UPDATE_STOP;

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
	if (!IsPlayMode() && !App->GetScene()->IsPrefabScene())
	{
		editor->SaveSettings();
	}
	bool ret = true;

	for (int i = NUM_MODULES - 1; i != 0 ; --i)
		ret = modules[i]->CleanUp();

	return ret;
}

void EngineApplication::Start()
{
	mIsPlayMode = true;

	SetCurrentClock(EngineApp->GetGameClock());
	scene->Save(std::string("InternalAssets/Scenes/" + std::string("TemporalScene")).c_str());	//TODO: Change to Importfile
	engineScriptManager->StartScripts();
	mGameTimer->Start();			
}

void EngineApplication::Stop()
{
	mIsPlayMode = false;

	mEngineTimer->SetTotalFrames(EngineApp->GetGameClock()->GetTotalFrames());
	mGameTimer->Stop();
	SetCurrentClock(EngineApp->GetEngineClock());
	mEngineTimer->Resume();
	EngineApp->GetAudio()->EngineStop();
	scene->Load(std::string("InternalAssets/Scenes/" + std::string("TemporalScene")).c_str());	//TODO: Change to Request Resource

}

float EngineApplication::GetRealDt() const
{
	return mEngineTimer->GetDelta() / (float)1000;
}

float EngineApplication::GetGameDt() const
{
	return mGameTimer->GetDelta() / (float)1000;
}
