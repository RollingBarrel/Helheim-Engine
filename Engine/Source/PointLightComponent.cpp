#include "Application.h"
#include "ModuleOpenGL.h"

#include "PointLightComponent.h"

PointLightComponent::PointLightComponent(GameObject* owner) : Component(owner, ComponentType::POINTLIGHT) 
{
	const float3& pos = owner->GetWorldPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;

	mData.radius = 25.0f;

	mData.color[0] = 1.0f;
	mData.color[1] = 1.0f;
	mData.color[2] = 1.0f;

	mData.intensity = 50.0f;
	
	if (IsEnabled())
	{
		App->GetOpenGL()->AddPointLight(*this);
	}
}

PointLightComponent::PointLightComponent(const PointLightComponent* original, GameObject* owner) : Component(owner, ComponentType::POINTLIGHT), mData(original->mData)
{
	if (IsEnabled())
	{
		App->GetOpenGL()->AddPointLight(*this);
	}
}

PointLightComponent::~PointLightComponent() 
{ 
	App->GetOpenGL()->RemovePointLight(*this); 
}

const float* PointLightComponent::GetPosition() const 
{ 
	return mData.pos; 
}

void PointLightComponent::SetColor(float col[3])
{
	mData.color[0] = col[0];
	mData.color[1] = col[1];
	mData.color[2] = col[2];
	App->GetOpenGL()->UpdatePointLightInfo(*this);
}

void PointLightComponent::SetIntensity(float intensity)
{
	mData.intensity = intensity;
	App->GetOpenGL()->UpdatePointLightInfo(*this);
}

void PointLightComponent::SetRadius(float radius)
{
	mData.radius = radius;
	App->GetOpenGL()->UpdatePointLightInfo(*this);
}

void PointLightComponent::Update()
{
	if (mOwner->HasUpdatedTransform())
	{
		float3 newPos = mOwner->GetWorldPosition();
		mData.pos[0] = newPos.x;
		mData.pos[1] = newPos.y;
		mData.pos[2] = newPos.z;
		App->GetOpenGL()->UpdatePointLightInfo(*this);
	}
}

inline Component* PointLightComponent::Clone(GameObject* owner) const 
{
	return new PointLightComponent(this, owner);
}

void PointLightComponent::Save(JsonObject& obj) const 
{
	Component::Save(obj);
	obj.AddFloats("Position", mData.pos, 3);
	obj.AddFloat("Radius", mData.radius);
	obj.AddFloats("Color", mData.color, 3);
	obj.AddFloat("Intensity", mData.intensity);
}

//TODO: why is the GO owner passed here??
void PointLightComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);

	if (data.HasMember("Position"))
	{
		float pos[3];
		data.GetFloats("Position", pos);
		for (unsigned int i = 0; i < 3; ++i)
		{
			mData.pos[i] = pos[i];
		}
	}

	if (data.HasMember("Radius"))
	{
		mData.radius = data.GetFloat("Radius");
		if (data.HasMember("Color"))
		{
			float color[3];
			data.GetFloats("Color", color);
			for (unsigned int i = 0; i < 3; ++i)
			{
				mData.color[i] = color[i];
			}
		}
	}

	if (data.HasMember("Intensity"))
	{
		mData.intensity = data.GetFloat("Intensity");
	}

	App->GetOpenGL()->UpdatePointLightInfo(*this);
}

void PointLightComponent::Reset()
{
	const float3& pos = mOwner->GetWorldPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;

	mData.radius = 25.0f;

	mData.color[0] = 1.0f;
	mData.color[1] = 1.0f;
	mData.color[2] = 1.0f;

	mData.intensity = 50.0f;

	App->GetOpenGL()->UpdatePointLightInfo(*this);
}

void PointLightComponent::Enable()
{
	App->GetOpenGL()->AddPointLight(*this);
}

void PointLightComponent::Disable()
{
	App->GetOpenGL()->RemovePointLight(*this);
}