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

void NavMeshObstacleComponent::Save(Archive& archive) const
{
	//archive.AddFloat("Radius", mRadius);
	//archive.AddFloat("Height", mHeight);
}

void NavMeshObstacleComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
	float radius{ 0.0f };
	float height{ 0.0f };
	if (data.HasMember("Radius") && data["Radius"].IsFloat()) 
	{
		radius = data["Radius"].GetFloat();
	}

	if (data.HasMember("Height") && data["Height"].IsFloat()) 
	{
		height = data["Height"].GetFloat();
	}

	mRadius = radius;
	mHeight = height;
}
