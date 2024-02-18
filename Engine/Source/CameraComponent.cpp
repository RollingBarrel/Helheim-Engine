#include "CameraComponent.h"

#include "Math/MathConstants.h"
#include "ModuleWindow.h"
#include "Application.h"
#include "CameraUtils.h"

#include "ModuleDebugDraw.h"

CameraComponent::CameraComponent(GameObject* owner)
	:Component(owner, ComponentType::CAMERA)
{
	mCamera = CameraUtils::InitiateCamera(float3(0.0f, 0.0f, 0.0f));
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
	App->GetDebugDraw()->DrawFrustum(*mCamera);
}

Component* CameraComponent::Clone(GameObject* owner) const
{
	return new CameraComponent(*this, owner);
}

void CameraComponent::Reset()
{
	mCamera = CameraUtils::InitiateCamera(float3(0.0f, 0.0f, 0.0f));
}

void CameraComponent::SetPosition(const float3& position)
{
	CameraUtils::CameraComponentTransform(position, *mCamera);
}

void CameraComponent::SetRotation(const float3& rotation)
{
	CameraUtils::Rotate(rotation,-1 ,*mCamera);
}

void CameraComponent::Save(Archive& archive) const
{
	//archive.AddInt("ID", mID);
	//archive.AddInt("Camera", mCamera);
}

void CameraComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
