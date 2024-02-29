#include "NavMeshObstacleComponent.h"

NavMeshObstacleComponent::NavMeshObstacleComponent(GameObject* ownerGameObject)
	:Component(ownerGameObject, ComponentType::NAVMESHOBSTACLE)
{
	
}

NavMeshObstacleComponent::~NavMeshObstacleComponent()
{
}

void NavMeshObstacleComponent::Update()
{
}

void NavMeshObstacleComponent::Reset()
{
}

Component* NavMeshObstacleComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void NavMeshObstacleComponent::Save(Archive& archive) const
{
}

void NavMeshObstacleComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
