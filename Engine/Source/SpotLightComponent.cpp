#include "SpotLightComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleDebugDraw.h"

SpotLightComponent::SpotLightComponent(GameObject* owner, const SpotLight& light) : Component(owner, ComponentType::SPOTLIGHT), mData(light) {
	const float3& pos = owner->GetWorldPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;
}

SpotLightComponent::~SpotLightComponent() { App->GetOpenGL()->RemovePointLight(*this); }

const float* SpotLightComponent::GetPosition() const 
{ 
	return mOwner->GetWorldPosition().ptr(); 
}

void SpotLightComponent::SetPosition(const float pos[3])
{
	mData.pos[0] = pos[0];
	mData.pos[1] = pos[1];
	mData.pos[2] = pos[2];
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void SpotLightComponent::SetColor(float col[3])
{
	mData.col[0] = col[0];
	mData.col[1] = col[1];
	mData.col[2] = col[2];
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void SpotLightComponent::SetIntensity(float intensity)
{
	mData.col[3] = intensity;
	App->GetOpenGL()->UpdatePoinLightInfo(*this);
}

void SpotLightComponent::SetRadius(float radius)
{
	mRadius = radius;
	App->GetOpenGL()->UpdatePoinLightInfo(*this, radius);
}

void SpotLightComponent::Update()
{
	//TODO: No mirarlo cada frame ??
	const float* pos = mOwner->GetWorldPosition().ptr();
	for (int i = 0; i < 3; ++i)
	{
		if (pos[i] != mData.pos[i])
		{
			SetPosition(pos);
		}
	}
	if (debugDraw)
	{
		App->GetDebugDraw()->DrawLineSphere(mData.pos, mData.col, mData.pos[3]);
	}
}