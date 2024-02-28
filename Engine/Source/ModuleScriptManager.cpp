#include "ModuleScriptManager.h"
#include "Script.h"
#include <Windows.h>

ModuleScriptManager::ModuleScriptManager()
{
}

ModuleScriptManager::~ModuleScriptManager()
{
}

bool ModuleScriptManager::Init()
{

	mHandle = LoadLibrary("Scripting.dll");

	Script*(*script)() = (Script* (*)())GetProcAddress(static_cast<HMODULE>(mHandle), "CreateTestScript");
	
	if (script != nullptr) {
		mScripts.push_back(script());
	}	
	else {
		LOG("Algo se ha muerto leyendo scripts");;
	}

	return true;

}

update_status ModuleScriptManager::PreUpdate(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleScriptManager::Update(float dt)
{

	for (std::vector<Script*>::iterator::value_type script : mScripts) {
		script->Update();
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

void ModuleScriptManager::Start()
{
	for (std::vector<Script*>::iterator::value_type script : mScripts) {

		script->Start();
	}
}


