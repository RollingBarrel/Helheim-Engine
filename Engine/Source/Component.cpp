#include "Component.h"
#include "GameObject.h"
#include "Algorithm/Random/LCG.h"

Component::Component(GameObject* owner, ComponentType type): mOwner(owner), mType(type), mID(LCG().Int()){}

const char* Component::GetNameFromType() const
{
	switch (mType)
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
		case ComponentType::NAVMESHCONTROLLER:
			return "NavMeshController";
		default:
			return "None";
	}
}