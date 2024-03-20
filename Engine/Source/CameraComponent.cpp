#include "CameraComponent.h"

#include "Math/MathConstants.h"
#include "Math/MathFunc.h"
#include "Application.h"

#include "ModuleDebugDraw.h"
#include "ModuleWindow.h"
#include "ModuleOpenGl.h"


CameraComponent::CameraComponent(GameObject* owner)
	:Component(owner, ComponentType::CAMERA)
{

    mFrustum.pos = owner->GetPosition();
    mFrustum.type = FrustumType::PerspectiveFrustum;
    mFrustum.nearPlaneDistance = 0.01f;
    mFrustum.farPlaneDistance = 1000.0f;

    float w = static_cast<float>(App->GetWindow()->GetWidth());
    float h = static_cast<float>(App->GetWindow()->GetHeight());
    mAspectRatio = w / h;
    mFrustum.verticalFov = math::pi / 4.0f;
    mFrustum.horizontalFov = 2.f * atanf(tanf(mFrustum.verticalFov * 0.5f) * mAspectRatio);

    LookAt(mFrustum.pos, mFrustum.pos + owner->GetFront(), float3::unitY);
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
    
}

Component* CameraComponent::Clone(GameObject* owner) const
{
	return new CameraComponent(*this, owner);
}

void CameraComponent::Reset()
{
	//mCamera = CameraUtils::InitiateCamera(float3(0.0f, 0.0f, 0.0f));
}

void CameraComponent::SetPosition(const float3& position)
{
	//CameraUtils::CameraComponentTransform(position, *mCamera);
}

void CameraComponent::SetRotation(const float3& rotation)
{
	//CameraUtils::Rotate(rotation,-1 ,*mCamera);
    float3 difference = GetOwner()->GetRotation() - rotation;

}

void CameraComponent::SetFOV(const float value)
{
    mFrustum.horizontalFov = math::DegToRad(value);
    mFrustum.verticalFov = 2.f * atanf(tanf(mFrustum.horizontalFov * 0.5f) * (1.0f / mAspectRatio));
}

void CameraComponent::SetAspectRatio(const float value)
{
    mAspectRatio = value;
    mFrustum.verticalFov = 2.f * atanf(tanf(mFrustum.horizontalFov * 0.5f) * (1.0f / mAspectRatio));
}

void CameraComponent::LookAt(float3 eyePos, float3 targetPos, float3 upVector)
{
    float3 forward = (targetPos - eyePos);
    forward.Normalize();
    float3 right = math::Cross(forward, upVector);
    right.Normalize();
    float3 up = math::Cross(right, forward);
    up.Normalize();

    mFrustum.pos = eyePos;
    mFrustum.front = forward;
    mFrustum.up = up;

    App->GetOpenGL()->SetOpenGlCameraUniforms();
}

void CameraComponent::Rotate(const float3& axis, float angleRad)
{
    float3x3 rotationMatrix = float3x3::RotateAxisAngle(axis, angleRad);
    mFrustum.up = rotationMatrix.Mul(mFrustum.up);
    mFrustum.front = rotationMatrix.Mul(mFrustum.front);

    App->GetOpenGL()->SetOpenGlCameraUniforms();
}

void CameraComponent::Transform(float3 vec)
{
    vec.z = -vec.z;
    float3x4 world = mFrustum.WorldMatrix();
    float3 newTrans = world.TransformDir(vec);
    world.SetTranslatePart(world.TranslatePart() + newTrans);
    mFrustum.SetWorldMatrix(world);

    App->GetOpenGL()->SetOpenGlCameraUniforms();
}

void CameraComponent::CameraComponentTransform(float3 vec)
{
    float3x4 world = mFrustum.WorldMatrix();
    float3 newTrans = world.TransformDir(vec);
    world.SetTranslatePart(world.TranslatePart() + newTrans);
    mFrustum.SetWorldMatrix(world);

    App->GetOpenGL()->SetOpenGlCameraUniforms();
}

void CameraComponent::Save(Archive& archive) const
{
    //archive.AddInt("ID", mID);
    //archive.AddInt("Camera", mCamera);
}

void CameraComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
