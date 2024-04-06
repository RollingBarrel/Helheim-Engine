#include "ModuleScriptManager.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Script.h"
#include <Windows.h>
#include <string>

ModuleScriptManager::ModuleScriptManager()
{
}

ModuleScriptManager::~ModuleScriptManager()
{
}

bool ModuleScriptManager::Init()
{

	mHandle = LoadLibrary("Scripting.dll");
	mLastModificationTime = App->GetFileSystem()->GetLastModTime("../Scripting/Output/Scripting.dll");
	return true;
}

update_status ModuleScriptManager::PreUpdate(float dt)
{
	int64_t modificationTime = App->GetFileSystem()->GetLastModTime("../Scripting/Output/Scripting.dll");
	if (mLastModificationTime != modificationTime)
	{
		mLastModificationTime = modificationTime;
		HotReload();
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScriptManager::Update(float dt)
{
	if (mIsPlaying) {
		for (std::vector<Script*>::iterator::value_type script : mScripts) {
			script->Update();
		}
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScriptManager::PostUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool ModuleScriptManager::CleanUp()
{
	FreeLibrary(static_cast<HMODULE>(mHandle));
	return true;
}

void ModuleScriptManager::AddScript(Script* script) 
{ 
	mScripts.push_back(script); 
}

void ModuleScriptManager::RemoveScript(Script* script)
{
	std::vector<Script*>::iterator deletePos = mScripts.end();
	for (std::vector<Script*>::iterator it = mScripts.begin(); it != mScripts.end(); ++it) {

		if (*it == script) {
			deletePos = it;
			break;
		}
		
	}
	if (deletePos != mScripts.end()) {
		mScripts.erase(deletePos);
	}
		
}

void ModuleScriptManager::HotReload()
{
	FreeLibrary(static_cast<HMODULE>(mHandle));
	while (CopyFile("../Scripting/Output/Scripting.dll", "Scripting.dll", false) == FALSE) {}
	mHandle = LoadLibrary("Scripting.dll");

}

void ModuleScriptManager::Play()
{
	mIsPlaying = true;
	Start();
}

void ModuleScriptManager::Stop()
{
	mIsPlaying = false;

}

void ModuleScriptManager::Start()
{
	for (std::vector<Script*>::iterator::value_type script : mScripts) {

		script->Start();
	}
}


