#include "GameApp.h"
#include "ModuleWindow.h"
#include "ModuleOpenGL.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleFileSystem.h"
#include "ModuleScriptManager.h"
#include "ModuleResource.h"
#include "ModuleDetourNavigation.h"
#include "ModuleResource.h"
#include "ModuleUI.h"
#include "ModuleAudio.h"
#include "ModuleEvent.h"
#include "ModuleGame.h"
#include "Timer.h"

GameApplication* GameApp = NULL;

GameApplication::GameApplication()
{
	GameApp = this;
	mCurrentTimer = new Timer();
	
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
}

GameApplication::~GameApplication()
{
	for (int i = NUM_MODULES - 1; i >= 0; --i) 
	{
		delete modules[i];
	}
	delete mCurrentTimer;
}

bool GameApplication::Init()
{
	mCurrentTimer->Start();

	bool ret = true;

	for (int i = 0; i < NUM_MODULES && ret == true; ++i)
		ret = modules[i]->Init();

	return false;
}

update_status GameApplication::Update(float dt)
{
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

bool GameApplication::CleanUp()
{
	bool ret = true;

	for (int i = 0; i < NUM_MODULES; ++i)
		ret = modules[i]->CleanUp();

	return ret;
}
