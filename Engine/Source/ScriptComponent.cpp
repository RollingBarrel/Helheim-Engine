#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleScriptManager.h"
#include <Windows.h>
#include "Script.h"

ScriptComponent::ScriptComponent(GameObject* owner) : Component(owner, ComponentType::SCRIPT)
{
	const char* name = "TestScript";

	Script* (*script)(GameObject*) = (Script * (*)(GameObject*))GetProcAddress(static_cast<HMODULE>(App->GetScriptManager()->GetDLLHandle()), (std::string("Create") + std::string(name)).c_str());

	if (script != nullptr) {
		mScript = script(owner);
		//mScript->mGameObject = owner;
		App->GetScriptManager()->AddScript(mScript);
	}
	else {
		LOG("LOADING SCRIPT ERROR");
	}
}

void ScriptComponent::Update()
{
}

Component* ScriptComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void ScriptComponent::Reset()
{
}

void::ScriptComponent::Save(Archive& archive) const
{

}

void::ScriptComponent::LoadFromJSON(const rapidjson::Value & data, GameObject * owner)
{
	
}