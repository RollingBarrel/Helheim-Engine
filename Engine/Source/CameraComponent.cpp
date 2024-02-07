#include "CameraComponent.h"

#include "Math/MathConstants.h"
#include "ModuleWindow.h"
#include "Application.h"

#include "ModuleDebugDraw.h"

CameraComponent::CameraComponent(GameObject* owner)
	:Component(owner, ComponentType::CAMERA)
{
	mCamera.type = FrustumType::PerspectiveFrustum;
	mCamera.nearPlaneDistance = 0.1f;
	mCamera.farPlaneDistance = 100.0f;
	mCamera.verticalFov = math::pi / 4.0f;
	int w = App->GetWindow()->GetWidth();
	int h = App->GetWindow()->GetHeight();
	mCamera.horizontalFov = 2.f * atanf(tanf(mCamera.verticalFov * 0.5f) * (float)w / (float)h);
	LookAt(float3(0.0f, 4.0f, 8.0f), float3(0.0f, 0.0f, 0.0f), float3::unitY);
}

CameraComponent::CameraComponent(const CameraComponent& original, GameObject* owner)
	:Component(owner, ComponentType::CAMERA)
{
	mCamera = original.mCamera;
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::Update()
{
	App->GetDebugDraw()->DrawFrustum(mCamera);
}

Component* CameraComponent::Clone(GameObject* owner) const
{
	return new CameraComponent(*this, owner);
}

void CameraComponent::LookAt(float3 eyePos, float3 targetPos, float3 upVector) {
	float3 forward = (targetPos - eyePos);
	forward.Normalize();
	float3 right = math::Cross(forward, upVector);
	right.Normalize();
	float3 up = math::Cross(right, forward);
	up.Normalize();

	mCamera.pos = eyePos;
	mCamera.front = forward;
	mCamera.up = up;
}

void CameraComponent::Save(Archive& archive) const
{
	//archive.AddInt("ID", mID);
	//archive.AddInt("Camera", mCamera);
}

void CameraComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
