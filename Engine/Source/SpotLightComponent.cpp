#include "Application.h"
#include "ModuleOpenGL.h"
//#include "ModuleDebugDraw.h"
#include "MathFunc.h"

#include "SpotLightComponent.h"

SpotLightComponent::SpotLightComponent(GameObject* owner, const SpotLight& light) : Component(owner, ComponentType::SPOTLIGHT), mData(light) 
{
	const float3& pos = owner->GetPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;

	App->GetOpenGL()->AddSpotLight(*this);
}

SpotLightComponent::~SpotLightComponent() { App->GetOpenGL()->RemoveSpotLight(*this); }

const float* SpotLightComponent::GetPosition() const 
{ 
	return mOwner->GetPosition().ptr(); 
}

void SpotLightComponent::SetPosition(const float pos[3])
{
	mData.pos[0] = pos[0];
	mData.pos[1] = pos[1];
	mData.pos[2] = pos[2];
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetDirection(const float dir[3])
{
	mData.aimD[0] = dir[0];
	mData.aimD[1] = dir[1];
	mData.aimD[2] = dir[2];
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetColor(float col[3])
{
	mData.col[0] = col[0];
	mData.col[1] = col[1];
	mData.col[2] = col[2];
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetIntensity(float intensity)
{
	mData.pos[3] = intensity;
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetRadius(float radius)
{
	mData.radius = radius;
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

float SpotLightComponent::GetOuterAngle() const {
	return acos(mData.col[3]);
}

void SpotLightComponent::SetOuterAngle(float angle)
{
	mData.col[3] = cos(angle);
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

float SpotLightComponent::GetInnerAngle() const {
	return acos(mData.aimD[3]);
}

void SpotLightComponent::SetInnerAngle(float angle)
{
	mData.aimD[3] = cos(angle);
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::Update()
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
	//	App->GetDebugDraw()->DrawCone(mData.pos, mData.aimD, mData.col, mData.radius);
	//}
}

inline Component* SpotLightComponent::Clone(GameObject* owner) const 
{ 
	return new SpotLightComponent(owner, mData);
}

void SpotLightComponent::Save(JsonObject& obj) const 
{
	Component::Save(obj);
	obj.AddFloats("Position", mData.pos, 4);
	obj.AddFloats("Direction", mData.aimD, 4);
	obj.AddFloats("Color", mData.col, 4);
	obj.AddFloat("Radius", mData.radius);
}

//TODO: why is the GO owner passed here??
void SpotLightComponent::Load(const JsonObject& data) 
{	
	Component::Load(data);

	float pos[4];
	data.GetFloats("Position", pos);
	for (unsigned int i = 0; i < 4; ++i)
	{
		mData.pos[i] = pos[i];
	}
	float dir[4];
	data.GetFloats("Direction", dir);
	for (unsigned int i = 0; i < 4; ++i)
	{
		mData.aimD[i] = dir[i];
	}
	float col[4];
	data.GetFloats("Color", col);
	for (unsigned int i = 0; i < 4; ++i)
	{
		mData.col[i] = col[i];
	}

	mData.radius = data.GetFloat("Radius");
}

void SpotLightComponent::Enable()
{
	App->GetOpenGL()->AddSpotLight(*this);
}

void SpotLightComponent::Disable()
{
	App->GetOpenGL()->RemoveSpotLight(*this);
}