#include "Component.h"
#include "GameObject.h"
#include "Algorithm/Random/LCG.h"

Component::Component(GameObject* owner, ComponentType type): mOwner(owner), mType(type), mID(LCG().Int()){}

void Component::Save(Archive& archive) const
{
	archive.AddInt("ID", GetID());
	archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddBool("isEnabled", IsEnabled());
}

void Component::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
	if (data.HasMember("ID") && data["ID"].IsInt())
	{
		mID = data["ID"].GetInt();
	}
	if (data.HasMember("ComponentType") && data["ComponentType"].IsInt())
	{
		mType = static_cast<ComponentType>(data["ComponentType"].GetInt());
	}
	if (data.HasMember("isEnabled") && data["isEnabled"].IsBool())
	{
		mIsEnabled = data["isEnabled"].GetBool();
	}
}

const char* Component::GetNameFromType(ComponentType type)
{
	switch (type)
	{
		case ComponentType::MESHRENDERER:
			return "Mesh Renderer";
		case ComponentType::POINTLIGHT:
			return "Point Light";
		case ComponentType::SPOTLIGHT:
			return "Spot Light";
		case ComponentType::TEST:
			return "Test";
		case ComponentType::CAMERA:
			return "Camera";
		case ComponentType::SCRIPT:
			return "Script";
		case ComponentType::AIAGENT:
			return "AI Agent";
		case ComponentType::NAVMESHOBSTACLE:
			return "NavMeshObstacle";
		case ComponentType::CANVAS:
			return "Canvas";
		case ComponentType::TRANSFORM2D:
			return "Transform2D";
		case ComponentType::ANIMATION:
			return "Animation";
		case ComponentType::IMAGE:
			return "Image";
		case ComponentType::BUTTON:
			return "Button";
		case ComponentType::TEXT:
			return "Text";
		default:
			return "None";
		}
}
