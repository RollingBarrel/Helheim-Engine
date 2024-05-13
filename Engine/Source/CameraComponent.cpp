#include "CameraComponent.h"
#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleOpenGl.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"

#include "Quadtree.h"
#include "MeshRendererComponent.h"

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
        float3 position = mOwner->GetWorldPosition();
        mFrustum.pos = position;


        mFrustum.pos = mOwner->GetPosition();
        mFrustum.front = mOwner->GetFront();
        mFrustum.up = mOwner->GetUp();
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
}

void CameraComponent::SetAspectRatio(float value)
{
    mAspectRatio = value;
    mFrustum.verticalFov = 2.f * atanf(tanf(mFrustum.horizontalFov * 0.5f) * (1.0f / mAspectRatio));
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
    
    App->GetOpenGL()->SetOpenGlCameraUniforms();
}
