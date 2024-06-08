#include "NavMeshObstacleComponent.h"
#include "GameObject.h"

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

void NavMeshObstacleComponent::Save(JsonObject& obj) const
{
	obj.AddFloat("Radius", mRadius);
	obj.AddFloat("Height", mHeight);
}

void NavMeshObstacleComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);

	mRadius = data.GetFloat("Radius");
	mHeight = data.GetFloat("Height");
}
