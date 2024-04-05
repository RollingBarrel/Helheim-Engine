#include "CameraComponent.h"

#include "Math/MathConstants.h"
#include "Math/MathFunc.h"
#include "Application.h"

#include "ModuleDebugDraw.h"
#include "ModuleWindow.h"
#include "ModuleOpenGl.h"
#include "ModuleCamera.h"

#include "ModuleScene.h"
#include "Quadtree.h"
#include "MeshRendererComponent.h"

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
    mAspectRatio = original.mFrustum.AspectRatio();
}

CameraComponent::~CameraComponent()
{
    if (this == App->GetCamera()->GetCurrentCamera() && App->GetCamera()->GetEditorCamera() != nullptr)
    {
        App->GetCamera()->SetCurrentCamera(App->GetCamera()->GetEditorCamera()->mOwner); //Change to create a function that looks for other camera components before setting the editor camera for ingame.
    }
}

void CameraComponent::Update()
{
    if (mOwner->GetHasBeenUpdated() && App->GetCamera()->GetCurrentCamera() != this)
    {
        float3 rotation = mOwner->GetRotation();
        float3 position = mOwner->GetPosition();

        float pos_angle_rotated = rotation.MaxElement();
        float neg_angle_rotated = rotation.MinElement();
        float angle_rotated = 1.0;
        bool has_rotated = false;
        if (pos_angle_rotated != 0) {
            angle_rotated = pos_angle_rotated;
            has_rotated = true;
        }
        else if (neg_angle_rotated != 0) {
            angle_rotated = neg_angle_rotated;
            has_rotated = true;
        }

        SetPos(position);
        if (has_rotated)
        {

            float3x3 rotationMatrix = float3x3::RotateAxisAngle(rotation.Abs().Normalized(), angle_rotated);
            mFrustum.up = rotationMatrix.Mul(float3::unitY);
            mFrustum.front = rotationMatrix.Mul(float3::unitZ);

        }
        
        App->GetOpenGL()->SetOpenGlCameraUniforms();

    }

    //Frustum culling updates
    App->GetScene()->ResetFrustumCulling(App->GetScene()->GetRoot());
    std::set<GameObject*> drawableObjects = App->GetScene()->GetQuadtreeRoot()->GetObjectsInFrustum(&mFrustum);

    for (const auto& object : drawableObjects)
    {
        MeshRendererComponent* meshComponent = (MeshRendererComponent*)object->GetComponent(ComponentType::MESHRENDERER);
        if (meshComponent != nullptr)
        {
            meshComponent->SetInsideFrustum(true);
        }
    }

}

Component* CameraComponent::Clone(GameObject* owner) const
{
	return new CameraComponent(*this, owner);
}

void CameraComponent::Reset()
{
	//mCamera = CameraUtils::InitiateCamera(float3(0.0f, 0.0f, 0.0f));
}


void CameraComponent::SetRotation(const float3& rotation)
{
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
