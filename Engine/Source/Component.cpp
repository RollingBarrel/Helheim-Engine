#include "Component.h"
#include "GameObject.h"
#include "Algorithm/Random/LCG.h"

Component::Component(GameObject* owner, ComponentType type): mOwner(owner), mType(type), mID(LCG().Int()){}

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
		case ComponentType::AUDIOLISTENER:
			return "Audio Listener";
		case ComponentType::AUDIOSOURCE:
			return "Audio Source";
		default:
			return "None";
		}
}
