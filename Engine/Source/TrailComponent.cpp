#include "TrailComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "Trail.h"

TrailComponent::TrailComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::PARTICLESYSTEM)
{
    Init();
}

TrailComponent::TrailComponent(const TrailComponent& original, GameObject* owner) : Component(owner, ComponentType::PARTICLESYSTEM), 
mResourceId(original.mResourceId), mFileName(original.mFileName), mTrail(new Trail(*original.mTrail))
{
    Init();
}

TrailComponent::~TrailComponent()
{
    delete mTrail;
}

Component* TrailComponent::Clone(GameObject* owner) const
{
    return new TrailComponent(*this, owner);
}



void TrailComponent::Init()
{
    SetImage(mResourceId);
    mTrail->Init();
}

void TrailComponent::Draw() 
{
    if (IsEnabled()) 
    {
        mTrail->Draw();
    }
}

void TrailComponent::Update()
{
    if (IsEnabled())
    {
        float3 position = mOwner->GetPosition();
        float dposition = position.DistanceSq(mTrail->GetLastPosition());
        if (dposition >= mMinDistance * mMinDistance && mTrail->GetSize() < mMaxPoints)
        {
            mTrail->AddTrailPositions(position, mOwner->GetRotation());
        }
        mTrail->Update();
    }
}

void TrailComponent::SetImage(unsigned int resourceId)
{
    mResourceId = resourceId;
    auto image = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
    mTrail->SetImage(image);
}

void TrailComponent::Reset()
{
    delete mTrail;
    *this = TrailComponent(mOwner);
}

void TrailComponent::Save(Archive& archive) const
{
    Component::Save(archive);
    archive.AddInt("Image", mResourceId);
    archive.AddInt("Max Points", mMaxPoints);
    archive.AddFloat("Min Distance", mMinDistance);
    mTrail->SaveJson(archive);
}

void TrailComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    Component::LoadFromJSON(data, owner);
    if (data.HasMember("Image") && data["Image"].IsInt())
    {
        mResourceId = data["Image"].GetInt();
        SetImage(mResourceId);
    }
    if (data.HasMember("Max Points") && data["Max Points"].IsInt())
    {
        mMaxPoints = data["Max Points"].GetInt();
    }
    if (data.HasMember("Min Distance") && data["Min Distance"].IsFloat())
    {
        mMinDistance = data["Min Distance"].GetFloat();
    }
    mTrail->LoadJson(data);
    Init();
}

void TrailComponent::Enable()
{
    App->GetOpenGL()->AddTrail(mTrail);
}

void TrailComponent::Disable()
{
    App->GetOpenGL()->RemoveTrail(mTrail);
}
