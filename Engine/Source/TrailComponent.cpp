#include "TrailComponent.h"

TrailComponent::TrailComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::PARTICLESYSTEM)
{
    SetImage(mResourceId);
    mColorGradient = new ColorGradient();
    mColorGradient->AddColorGradientMark(0.5f, float4(1.0f, 0.0f, 0.0f, 1.0f));
    Init();
}

TrailComponent::TrailComponent(const TrailComponent& original, GameObject* owner) :
    mMaxLifeTime(original.mMaxLifeTime), mFileName(original.mFileName),
    mIsSizeCurve(original.mIsSizeCurve), mSizeCurve(original.mSizeCurve),
    mSizeLineal(original.mSizeLineal),
    mColorGradient(original.mColorGradient), Component(owner, ComponentType::PARTICLESYSTEM)
{
    SetImage(original.mResourceId);
    Init();
}

TrailComponent::~TrailComponent()
{
    App->GetOpenGL()->RemoveParticleSystem(this);
    glDeleteBuffers(1, &mInstanceBuffer);
    glDeleteBuffers(1, &mVBO);
    delete mColorGradient;
}

Component* TrailComponent::Clone(GameObject* owner) const
{
    return new TrailComponent(*this, owner);
}



void TrailComponent::Init()
{

}

void TrailComponent::Draw() const
{

}

void TrailComponent::Update()
{
    mTrail.Update();
    float3 camPosition = App->GetCamera()->GetCurrentCamera()->GetFrustum().pos;

}

void TrailComponent::SetImage(unsigned int resourceId)
{
    mResourceId = resourceId;
    mImage = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
}

void TrailComponent::Reset()
{

}

void TrailComponent::Save(Archive& archive) const
{
    Component::Save(archive);
    archive.AddInt("Image", mResourceId);
}

void TrailComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    Component::LoadFromJSON(data, owner);
    if (data.HasMember("Duration") && data["Duration"].IsFloat())
    {
        mDuration = data["Duration"].GetFloat();
    }
    if (data.HasMember("Image") && data["Image"].IsInt())
    {
        mResourceId = data["Image"].GetInt();
        SetImage(mResourceId);
    }
    if (data.HasMember("Life Time") && data["Life Time"].IsFloat())
    {
        mMaxLifeTime = data["Life Time"].GetFloat();
    }
}

