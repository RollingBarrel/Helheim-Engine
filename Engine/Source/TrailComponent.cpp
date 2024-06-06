#include "TrailComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "Trail.h"

TrailComponent::TrailComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::TRAIL)
{
    mTrail = new Trail();
}

TrailComponent::TrailComponent(const TrailComponent& original, GameObject* owner) : Component(owner, ComponentType::TRAIL), 
mResourceId(original.mResourceId), mFileName(original.mFileName), mTrail(new Trail(*original.mTrail)),
mMaxPoints(original.mMaxPoints), mMinDistance(original.mMinDistance)
{
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
    mTrail->Init();
    SetImage(mResourceId);
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
    //Init(); // THIS IS HORRIBLE HELP ME FIND A SOLUTION
    if (IsEnabled())
    {        
        float3 position, scale;
        Quat rotation;
        mOwner->GetWorldTransform().Decompose(position, rotation, scale);
        const float3 lastPosition = mTrail->GetLastPosition();
        const float dposition = position.DistanceSq(lastPosition);
        if (mTrail->GetSize() <= 0 || dposition >= mMinDistance * mMinDistance && mTrail->GetSize() < mMaxPoints)
        {
            mTrail->AddTrailPositions(position, rotation);
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

void TrailComponent::Save(JsonObject& obj) const
{
    Component::Save(obj);
    obj.AddInt("Image", mResourceId);
    obj.AddInt("Max Points", mMaxPoints);
    obj.AddFloat("Min Distance", mMinDistance);
    mTrail->Save(obj);
}

void TrailComponent::Load(const JsonObject& data)
{
    Component::Load(data);

    mResourceId = data.GetInt("Image");
    SetImage(mResourceId);
    mMaxPoints = data.GetInt("Mas Points");
    mMinDistance = data.GetFloat("Min Distance");    
    mTrail->Load(data);
    Init();
}

void TrailComponent::Enable()
{
    App->GetOpenGL()->AddTrail(mTrail);
    Init(); // THIS IS HORRIBLE HELP ME FIND A SOLUTION
}

void TrailComponent::Disable()
{
    App->GetOpenGL()->RemoveTrail(mTrail);
    mTrail->Clear();
}
