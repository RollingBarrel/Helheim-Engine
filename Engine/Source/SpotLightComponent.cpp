#include "SpotLightComponent.h"
#include "ModuleOpenGL.h"
#include "Application.h"
#include "glew.h"
#include "MathFunc.h"

SpotLightComponent::SpotLightComponent(GameObject* owner) : Component(owner, ComponentType::SPOTLIGHT)
{
	const float3& pos = owner->GetPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;
	mData.pos[3] = 50.0f; //intensity

	const float3& dir = owner->GetFront();
	mData.aimD[0] = dir.x;
	mData.aimD[1] = dir.y;
	mData.aimD[2] = dir.z;
	mData.aimD[3] = cos(DegToRad(25.0f)); //cos inner angle

	mData.color[0] = 1.0f;
	mData.color[1] = 1.0f;
	mData.color[2] = 1.0f;
	mData.color[3] = cos(DegToRad(38.0f)); //cos outer angle

	mData.range = 15.0f;
	mBias = 0.00001f;

	mShadowFrustum.type = FrustumType::PerspectiveFrustum;
	mShadowFrustum.pos = owner->GetPosition();
	mShadowFrustum.front = owner->GetFront();
	mShadowFrustum.up = owner->GetUp();
	mShadowFrustum.nearPlaneDistance = 0.1f;
	mShadowFrustum.farPlaneDistance = mData.range;
	mShadowFrustum.horizontalFov = 2.0f * acos(mData.color[3]);
	mShadowFrustum.verticalFov = 2.0f * acos(mData.color[3]);

	App->GetOpenGL()->AddSpotLight(*this);
}


SpotLightComponent::SpotLightComponent(const SpotLightComponent* original, GameObject* owner)
	: Component(owner, ComponentType::SPOTLIGHT), mData(original->mData), mShadowFrustum(original->mShadowFrustum), mCastShadow(original->mCastShadow), mBias(original->mBias)
{
	App->GetOpenGL()->AddSpotLight(*this);
}

SpotLightComponent::~SpotLightComponent()
{
	App->GetOpenGL()->RemoveSpotLight(*this);
}

const float* SpotLightComponent::GetPosition() const
{
	return mOwner->GetPosition().ptr();
}

void SpotLightComponent::SetColor(float color[3])
{
	mData.color[0] = color[0];
	mData.color[1] = color[1];
	mData.color[2] = color[2];
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetIntensity(float intensity)
{
	mData.pos[3] = intensity;
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::SetRange(float range)
{
	mData.range = range;
	mShadowFrustum.farPlaneDistance = range;
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

float SpotLightComponent::GetOuterAngle() const {
	return acos(mData.color[3]);
}

void SpotLightComponent::SetOuterAngle(float angle)
{
	mData.color[3] = cos(angle);
	mShadowFrustum.horizontalFov = 2.0f * angle;
	mShadowFrustum.verticalFov = 2.0f * angle;
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

void SpotLightComponent::SetBias(float bias)
{
	mBias = bias;
}

inline void SpotLightComponent::SetShadowIndex(int index)
{ 
	mData.shadowIndex = index;
	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}


void SpotLightComponent::Update()
{
	if (mOwner->HasUpdatedTransform())
	{
		float3 newPosition = mOwner->GetPosition();
		mData.pos[0] = newPosition[0];
		mData.pos[1] = newPosition[1];
		mData.pos[2] = newPosition[2];

		float3 newDirection = mOwner->GetFront();
		mData.aimD[0] = newDirection[0];
		mData.aimD[1] = newDirection[1];
		mData.aimD[2] = newDirection[2];

		mShadowFrustum.pos = newPosition;
		mShadowFrustum.front = newDirection;
		mShadowFrustum.up = mOwner->GetUp();

		App->GetOpenGL()->UpdateSpotLightInfo(*this);
	}
}

inline Component* SpotLightComponent::Clone(GameObject* owner) const
{
	return new SpotLightComponent(this, owner);
}

void SpotLightComponent::Save(JsonObject& obj) const 
{
	Component::Save(obj);
	obj.AddFloats("Position", mData.pos, 4);
	obj.AddFloats("Direction", mData.aimD, 4);
	obj.AddFloats("Color", mData.color, 4);
	obj.AddFloat("Range", mData.range);
	obj.AddBool("CastShadow", mCastShadow);
	obj.AddFloat("Bias", mBias);
}

//TODO: why is the GO owner passed here??
void SpotLightComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{	
	Component::Load(data, uidPointerMap);

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
		mData.color[i] = col[i];
	}

	mData.range = data.GetFloat("Range");

	mCastShadow = data.GetBool("CastShadow");
	
	mBias = data.GetFloat("Bias");
	
	GameObject* owner = this->GetOwner();
	mShadowFrustum.pos = owner->GetPosition();
	mShadowFrustum.front = owner->GetFront();
	mShadowFrustum.up = owner->GetUp();
	mShadowFrustum.nearPlaneDistance = 0.01f;
	mShadowFrustum.farPlaneDistance = mData.range;
	mShadowFrustum.horizontalFov = 2.0f * acos(mData.color[3]);
	mShadowFrustum.verticalFov = 2.0f * acos(mData.color[3]);

	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::Reset()
{
	const float3& pos = mOwner->GetPosition();
	mData.pos[0] = pos.x;
	mData.pos[1] = pos.y;
	mData.pos[2] = pos.z;
	mData.pos[3] = 50.0f; //intensity

	const float3& dir = mOwner->GetFront();
	mData.aimD[0] = dir.x;
	mData.aimD[1] = dir.y;
	mData.aimD[2] = dir.z;
	mData.aimD[3] = cos(DegToRad(25.0f)); //cos inner angle

	mData.color[0] = 1.0f;
	mData.color[1] = 1.0f;
	mData.color[2] = 1.0f;
	mData.color[3] = cos(DegToRad(38.0f)); //cos outer angle

	mData.range = 15.0f;
	mBias = 0.00001f;

	mShadowFrustum.type = FrustumType::PerspectiveFrustum;
	mShadowFrustum.pos = mOwner->GetPosition();
	mShadowFrustum.front = mOwner->GetFront();
	mShadowFrustum.up = mOwner->GetUp();
	mShadowFrustum.nearPlaneDistance = 0.1f;
	mShadowFrustum.farPlaneDistance = mData.range;
	mShadowFrustum.horizontalFov = 2.0f * acos(mData.color[3]);
	mShadowFrustum.verticalFov = 2.0f * acos(mData.color[3]);

	App->GetOpenGL()->UpdateSpotLightInfo(*this);
}

void SpotLightComponent::Enable()
{
	App->GetOpenGL()->AddSpotLight(*this);
}

void SpotLightComponent::Disable()
{
	App->GetOpenGL()->RemoveSpotLight(*this);
}