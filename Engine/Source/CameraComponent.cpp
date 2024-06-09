#include "CameraComponent.h"
#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleOpenGl.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Quadtree.h"
#include "MeshRendererComponent.h"

#include "Math/MathConstants.h"
#include "Math/MathFunc.h"

CameraComponent::CameraComponent(GameObject* owner) :Component(owner, ComponentType::CAMERA)
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

    mFrustum.front = mOwner->GetFront();
    mFrustum.up = mOwner->GetUp();

    App->GetCamera()->AddEnabledCamera(this);
}

CameraComponent::CameraComponent(const CameraComponent& original, GameObject* owner)
	:Component(owner, ComponentType::CAMERA)
{
	mFrustum = original.mFrustum;
    mAspectRatio = original.mFrustum.AspectRatio();
    App->GetCamera()->AddEnabledCamera(this);
}

CameraComponent::~CameraComponent()
{
   App->GetCamera()->RemoveEnabledCamera(this); 
}

void CameraComponent::Enable()
{
    App->GetCamera()->AddEnabledCamera(this);
}

void CameraComponent::Disable()
{
    App->GetCamera()->RemoveEnabledCamera(this);
}

void CameraComponent::Update()
{
    if (mOwner->HasUpdatedTransform())
    {
        float3 position = mOwner->GetPosition();
        mFrustum.pos = position;


        mFrustum.pos = mOwner->GetPosition();
        mFrustum.front = mOwner->GetFront();
        mFrustum.up = mOwner->GetUp();
        App->GetOpenGL()->SetOpenGlCameraUniforms();

    }
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

void CameraComponent::SetFOV(float value)
{
    mFrustum.horizontalFov = math::DegToRad(value);
    mFrustum.verticalFov = 2.f * atanf(tanf(mFrustum.horizontalFov * 0.5f) * (1.0f / mAspectRatio));
    App->GetOpenGL()->SetOpenGlCameraUniforms();

}

void CameraComponent::SetAspectRatio(float value)
{
    mAspectRatio = value;
    mFrustum.verticalFov = 2.f * atanf(tanf(mFrustum.horizontalFov * 0.5f) * (1.0f / mAspectRatio));
    App->GetOpenGL()->SetOpenGlCameraUniforms();

}

void CameraComponent::SetNearPlane(float value) 
{
    mFrustum.nearPlaneDistance = value;
    App->GetOpenGL()->SetOpenGlCameraUniforms();
}
void CameraComponent::SetFarPlane(float value) 
{ 
    mFrustum.farPlaneDistance = value;
    App->GetOpenGL()->SetOpenGlCameraUniforms();

}

void CameraComponent::Save(JsonObject& obj) const
{
    Component::Save(obj);

    obj.AddFloat("AspectRatio", mAspectRatio);
    obj.AddFloat("FieldOfView", mFrustum.verticalFov);
    obj.AddFloat("NearPlane", mFrustum.nearPlaneDistance);
    obj.AddFloat("FarPlane", mFrustum.farPlaneDistance);
    obj.AddBool("IsOrtographic", mFrustum.type == FrustumType::OrthographicFrustum);
}

void CameraComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
    Component::Load(data, uidPointerMap);
    if (data.HasMember("AspectRatio"))
    {
        mAspectRatio = data.GetFloat("AspectRatio");
        SetAspectRatio(mAspectRatio);
    }
    if (data.HasMember("FieldOfView"))
    {
        SetFOV(data.GetFloat("FieldOfView"));
    }
    if (data.HasMember("NearPlane"))
    {
        mFrustum.nearPlaneDistance = data.GetFloat("NearPlane");
        SetNearPlane(mFrustum.nearPlaneDistance);
    }
    if (data.HasMember("FarPlane"))
    {
        mFrustum.farPlaneDistance = data.GetFloat("FarPlane");
        SetFarPlane(mFrustum.farPlaneDistance);
    }

    if (data.HasMember("IsOrtographic"))
    {
        if(data.GetBool("IsOrtographic"))   
            mFrustum.type = FrustumType::OrthographicFrustum;      
    }

    App->GetOpenGL()->SetOpenGlCameraUniforms();
}
