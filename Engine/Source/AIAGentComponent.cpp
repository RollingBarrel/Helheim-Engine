#include "AIAgentComponent.h"
#include "Application.h"
#include "ModuleDetourNavigation.h"
#include "GameObject.h"

AIAgentComponent::AIAgentComponent(GameObject* ownerGameObject)
 : Component(ownerGameObject, ComponentType::AIAGENT)
{
}

AIAgentComponent::AIAgentComponent(const AIAgentComponent& original, GameObject* owner)
	: Component(owner, ComponentType::AIAGENT)
{
}

AIAgentComponent::~AIAgentComponent()
{
}

Component* AIAgentComponent::Clone(GameObject* owner) const
{
	return new AIAgentComponent(*this, owner);
}

void AIAgentComponent::SetNavigationPath(const float3& destination) 
{
	mNavPositions = App->GetNavigation()->FindNavPath(GetOwner()->GetWorldPosition(), destination);
}

void AIAgentComponent::MoveAgent(float speed) const
{
	float3 direction = (mNavPositions[1] - mNavPositions[0]).Normalized();
	direction = direction / 50 * speed;
	GetOwner()->SetWorldPosition(GetOwner()->GetWorldPosition() + direction);
}

void AIAgentComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
}

void AIAgentComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);
}

