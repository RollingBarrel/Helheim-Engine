#include "ScriptComponent.h"

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