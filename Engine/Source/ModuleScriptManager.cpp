#include "ModuleScriptManager.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "Script.h"
#include "ScriptComponent.h"
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
		for (std::vector<ScriptComponent*>::iterator::value_type script : mScripts) {
			script->mScript->Update();
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

void ModuleScriptManager::AddScript(ScriptComponent* script) 
{ 
	mScripts.push_back(script); 
}

void ModuleScriptManager::RemoveScript(ScriptComponent* script)
{
	std::vector<ScriptComponent*>::iterator deletePos = mScripts.end();
	for (std::vector<ScriptComponent*>::iterator it = mScripts.begin(); it != mScripts.end(); ++it) {

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
	ReloadScripts();
}

void ModuleScriptManager::ReloadScripts()
{
	for (ScriptComponent* scriptComponent : mScripts) {

		Script* oldScript = scriptComponent->mScript;

		std::vector<Member> oldMembers = oldScript->Serialize();


		scriptComponent->LoadScript(scriptComponent->GetScriptName());

		Script* newScript = scriptComponent->mScript;

		std::vector<Member> newMembers = newScript->Serialize();
		
		
		for (int i = 0; i < oldMembers.size(); i++) 
		{

			for (int j = 0; j < newMembers.size(); j++) 
			{
				
				if (strcmp(oldMembers[i].mName, newMembers[j].mName) == 0) {
					char* newScriptPos = ((char*)newScript) + newMembers[j].mOffset;

					char* oldScriptPos = ((char*)oldScript) + oldMembers[i].mOffset;

					switch (oldMembers[i].mType) {
					case(MemberType::FLOAT):
						memcpy(newScriptPos, oldScriptPos, sizeof(float));
						break;
					}


				}

			}

		}

	}

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
	for (std::vector<ScriptComponent*>::iterator::value_type script : mScripts) {

		script->mScript->Start();
	}
}


