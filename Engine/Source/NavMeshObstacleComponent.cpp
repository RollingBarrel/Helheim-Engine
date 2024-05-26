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

void NavMeshObstacleComponent::Load(const JsonObject& data, GameObject* owner)
{
	Component::Load(data, owner);
	//float radius{ 0.0f };
	//float height{ 0.0f };
	//if (data.HasMember("Radius") && data["Radius"].IsFloat()) 
	//{
	//	radius = data["Radius"].GetFloat();
	//}
	//
	//if (data.HasMember("Height") && data["Height"].IsFloat()) 
	//{
	//	height = data["Height"].GetFloat();
	//}
	//
	//mRadius = radius;
	//mHeight = height;
}
