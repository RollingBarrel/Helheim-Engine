#include "Application.h"
#include "ModuleOpenGL.h"

#include "PointLightComponent.h"

PointLightComponent::PointLightComponent(GameObject* owner) : Component(owner, ComponentType::POINTLIGHT) 
{
	const float3& pos = owner->GetPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;

	mData.radius = 25.0f;

	mData.color[0] = 1.0f;
	mData.color[1] = 1.0f;
	mData.color[2] = 1.0f;

	mData.intensity = 50.0f;
	
	App->GetOpenGL()->AddPointLight(*this);
}

PointLightComponent::PointLightComponent(const PointLightComponent* original, GameObject* owner) : Component(owner, ComponentType::POINTLIGHT), mData(original->mData)
{
	App->GetOpenGL()->AddPointLight(*this);
}

PointLightComponent::~PointLightComponent() 
{ 
	App->GetOpenGL()->RemovePointLight(*this); 
}

const float* PointLightComponent::GetPosition() const 
{ 
	return mOwner->GetPosition().ptr(); 
}

void PointLightComponent::SetPosition(const float pos[3])
{
	mData.pos[0] = pos[0];
	mData.pos[1] = pos[1];
	mData.pos[2] = pos[2];
	App->GetOpenGL()->UpdatePointLightInfo(*this);
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
	//TODO: No mirarlo cada frame ??
	const float* pos = mOwner->GetPosition().ptr();
	for (int i = 0; i < 3; ++i)
	{
		if (pos[i] != mData.pos[i])
		{
			SetPosition(pos);
		}
	}
	//TODO: SEPARATE ENGINE
	//if (debugDraw)
	//{
	//	App->GetDebugDraw()->DrawSphere(mData.pos, mData.col, mData.pos[3]);
	//}
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
void PointLightComponent::Load(const JsonObject& data)
{
	Component::Load(data);

	float pos[3];
	data.GetFloats("Position", pos);
	for (unsigned int i = 0; i < 3; ++i)
	{
		mData.pos[i] = pos[i];
	}
	mData.radius = data.GetFloat("Radius");
	float color[3];
	data.GetFloats("Color", color);
	for (unsigned int i = 0; i < 3; ++i)
	{
		mData.color[i] = color[i];
	}
	mData.intensity = data.GetFloat("Intensity");
}

void PointLightComponent::Reset()
{
	const float3& pos = mOwner->GetPosition();
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