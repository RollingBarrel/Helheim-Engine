#include "DecalComponent.h"

DecalComponent::DecalComponent(GameObject* owner) : Component(owner, ComponentType::DECAL)
{

}

DecalComponent::DecalComponent(const DecalComponent& other, GameObject* owner) : Component(owner, ComponentType::DECAL)
{

}

DecalComponent::~DecalComponent()
{
}

void DecalComponent::Update()
{
}

Component* DecalComponent::Clone(GameObject* owner) const
{
	return new DecalComponent(*this, owner);
}

void DecalComponent::Save(JsonObject& obj) const
{
}

void DecalComponent::Load(const JsonObject& data)
{
}
