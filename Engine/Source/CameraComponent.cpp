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
    if (mOwner->HasUpdatedTransform())
    {
        float3 position = mOwner->GetWorldPosition();
        SetPos(position);


        //float3 rotation = mOwner->GetRotation();
        float3 rotation = mOwner->GetWorldTransform().ToEulerXYZ();


        float3x3 rotationMatrix = float3x3::RotateAxisAngle(float3(1,0,0), rotation.x);
        rotationMatrix = rotationMatrix * float3x3::RotateAxisAngle(float3(0, 1, 0), rotation.y);
        rotationMatrix = rotationMatrix * float3x3::RotateAxisAngle(float3(0, 0, 1), rotation.z);
  
        LookAt(mFrustum.pos, mFrustum.pos + rotationMatrix.Mul(float3::unitZ), rotationMatrix.Mul(float3::unitY));

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

    App->GetOpenGL()->BindSceneFramebuffer();
    App->GetDebugDraw()->DrawFrustum(mFrustum);
    App->GetOpenGL()->UnbindSceneFramebuffer();

}

Component* CameraComponent::Clone(GameObject* owner) const
{
	return new CameraComponent(*this, owner);
}

void CameraComponent::Reset()
{
    mFrustum.type = FrustumType::PerspectiveFrustum;
    mFrustum.nearPlaneDistance = 0.01f;
    mFrustum.farPlaneDistance = 1000.0f;

    float w = static_cast<float>(App->GetWindow()->GetWidth());
    float h = static_cast<float>(App->GetWindow()->GetHeight());
    mAspectRatio = w / h;
    mFrustum.verticalFov = math::pi / 4.0f;
    mFrustum.horizontalFov = 2.f * atanf(tanf(mFrustum.verticalFov * 0.5f) * mAspectRatio);

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
    archive.AddInt("ComponentType", static_cast<int>(GetType()));
    archive.AddFloat("AspectRatio", mAspectRatio);
    archive.AddFloat("NearPlane", mFrustum.nearPlaneDistance);
    archive.AddFloat("FarPlane", mFrustum.farPlaneDistance);
    archive.AddBool("IsOrtographic", mFrustum.type == FrustumType::OrthographicFrustum);

    
}

void CameraComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    
    float w = static_cast<float>(App->GetWindow()->GetWidth());
    float h = static_cast<float>(App->GetWindow()->GetHeight());

    float aspectRatio = { w/h };
    float nearPlane = { 0.01f };
    float farPlane = { 1000.0f };
    

    if (data.HasMember("AspectRatio") && data["AspectRatio"].IsFloat()) {
        aspectRatio = data["AspectRatio"].GetFloat();
    }
    if (data.HasMember("NearPlane") && data["NearPlane"].IsFloat()) {
        nearPlane = data["NearPlane"].GetFloat();
    }
    if (data.HasMember("FarPlane") && data["FarPlane"].IsFloat()) {
        farPlane = data["FarPlane"].GetFloat();
    }
    if (data.HasMember("IsOrtographic") && data["IsOrtographic"].IsBool()) {
        if (data["IsOrtographic"].GetBool())
            mFrustum.type = FrustumType::OrthographicFrustum;
    }

    SetAspectRatio(aspectRatio);
    SetNearPlane(nearPlane);
    SetFarPlane(farPlane);
    

}
