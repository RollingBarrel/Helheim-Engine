#include "LightSourceComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"

LightSourceComponent::LightSourceComponent(GameObject* owner, const PointLight& light) : Component(owner, ComponentType::LIGHTSOURCE), mData(light) {
	const float3& pos = owner->GetLocalPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;
}

LightSourceComponent::~LightSourceComponent() { App->GetOpenGL()->RemovePointLight(*this); }

const float* LightSourceComponent::GetPosition() const 
{ 
	return mOwner->GetLocalPosition().ptr(); 
}

void LightSourceComponent::SetPosition(const float pos[3])
{
	mData.pos[0] = pos[0];
	mData.pos[1] = pos[1];
	mData.pos[2] = pos[2];
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void LightSourceComponent::SetColor(float col[3])
{
	mData.col[0] = col[0];
	mData.col[1] = col[1];
	mData.col[2] = col[2];
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void LightSourceComponent::SetIntensity(float intensity)
{
	mData.col[3] = intensity;
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void LightSourceComponent::SetRadius(float radius)
{
	mData.pos[3] = radius;
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void LightSourceComponent::Update()
{
	//TODO: No mirarlo cada frame ??
	const float* pos = mOwner->GetLocalPosition().ptr();
	for (int i = 0; i < 3; ++i)
	{
		if (pos[i] != mData.pos[i])
		{
			SetPosition(pos);
		}
	}
}