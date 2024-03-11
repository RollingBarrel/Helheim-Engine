#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleScriptManager.h"
#include <Windows.h>
#include "Script.h"

ScriptComponent::ScriptComponent(GameObject* owner) : Component(owner, ComponentType::SCRIPT)
{

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

void ScriptComponent::LoadScript(const char* scriptName)
{
	// TODO: Deberia recibir el UID del resource y con el resorce se puede obtener el scriptName y los attributos a mostrar en el inspector
	mName = scriptName;

	Script* (*script)(GameObject*) = (Script * (*)(GameObject*))GetProcAddress(static_cast<HMODULE>(App->GetScriptManager()->GetDLLHandle()), (std::string("Create") + std::string(mName)).c_str());

	if (script != nullptr) {
		mScript = script(mOwner);
		//mScript->mGameObject = owner;
		App->GetScriptManager()->AddScript(mScript);
		LOG("LOADING SCRIPT SUCCESS");
	}
	else {
		LOG("LOADING SCRIPT ERROR");
	}
}
