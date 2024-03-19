#include "CameraComponent.h"

#include "ModuleWindow.h"
#include "ModuleDebugDraw.h"

#include "Math/MathConstants.h"
#include "Application.h"



CameraComponent::CameraComponent(GameObject* owner)
	:Component(owner, ComponentType::CAMERA)
{
	mFrustum.type = FrustumType::PerspectiveFrustum;
	mFrustum.nearPlaneDistance = 0.1f;
	mFrustum.farPlaneDistance = 2000.0f;
	mFrustum.verticalFov = math::pi / 4.0f;
	int w = App->GetWindow()->GetWidth();
	int h = App->GetWindow()->GetHeight();
	mFrustum.horizontalFov = 2.f * atanf(tanf(mFrustum.verticalFov * 0.5f) * (float)w / (float)h);

	//LookAt(float3(0.0f, 4.0f, 8.0f), float3(0.0f, 0.0f, 0.0f), float3::unitY);
}

CameraComponent::CameraComponent(const CameraComponent& original, GameObject* owner)
	:Component(owner, ComponentType::CAMERA)
{
	mFrustum = original.mFrustum;
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Update()
{
	//App->GetDebugDraw()->DrawFrustum(mFrustum);
}

Component* CameraComponent::Clone(GameObject* owner) const
{
	return new CameraComponent(*this, owner);
}

void CameraComponent::Reset()
{
	//mCamera = InitiateCamera(float3(0.0f, 0.0f, 0.0f));
}

void CameraComponent::SetPosition(const float3& position)
{
	//CameraUtils::CameraComponentTransform(position, *mCamera);
}

void CameraComponent::SetRotation(const float3& rotation)
{
	//CameraUtils::Rotate(rotation,-1 ,*mCamera);
}

void CameraComponent::Save(Archive& archive) const
{
	//archive.AddInt("ID", mID);
	//archive.AddInt("Camera", mCamera);
}

void CameraComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
