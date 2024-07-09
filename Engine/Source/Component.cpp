#include "Component.h"
#include "GameObject.h"
#include "Algorithm/Random/LCG.h"

Component::Component(GameObject* owner, ComponentType type): mOwner(owner), mType(type), mID(LCG().Int())
{
	mIsEnabled = mOwner->IsActive();
}

void Component::Save(JsonObject& obj) const
{
	//obj.AddInt("ID", GetID());
	obj.AddInt("ComponentType", static_cast<int>(GetType()));
	obj.AddBool("IsEnabled", IsEnabled());
}

void Component::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	if(data.HasMember("ComponentType"))
		mType = (ComponentType) data.GetInt("ComponentType");
	if(data.HasMember("IsEnabled"))
		mIsEnabled = data.GetBool("IsEnabled");
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
		case ComponentType::MASK:
			return "Mask";
		case ComponentType::BUTTON:
			return "Button";
		case ComponentType::AUDIOLISTENER:
			return "Audio Listener";
		case ComponentType::AUDIOSOURCE:
			return "Audio Source";
		case ComponentType::SLIDER:
			return "Slider";
		case ComponentType::TEXT:
			return "Text";
		case ComponentType::PARTICLESYSTEM:
			return "Particle System";
		case ComponentType::BOXCOLLIDER:
			return "Box Collider";
		case ComponentType::TRAIL:
			return "Trail";
		case ComponentType::DECAL:
			return "Decal Projector";
		default:
			return "None";
		}
}

void Component::SetEnable(bool enable)
{
	if (enable == mIsEnabled)
		return;
	mIsEnabled = enable;
	if (mIsEnabled)
		Enable();
	else
		Disable();
}
