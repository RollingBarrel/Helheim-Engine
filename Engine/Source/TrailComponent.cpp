//#include "TrailComponent.h"
//#include "Application.h"
//#include "ModuleOpenGL.h"
//#include "ModuleResource.h"
//#include "Trail.h"
//
//TrailComponent::TrailComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::PARTICLESYSTEM)
//{
//    SetImage(mResourceId);
//    mColorGradient = new ColorGradient();
//    mColorGradient->AddColorGradientMark(0.5f, float4(1.0f, 0.0f, 0.0f, 1.0f));
//    Init();
//}
//
//TrailComponent::TrailComponent(const TrailComponent& original, GameObject* owner) :
//    mMaxLifeTime(original.mMaxLifeTime), mFileName(original.mFileName),
//    mIsSizeCurve(original.mIsSizeCurve), mSizeCurve(original.mSizeCurve),
//    mSizeLineal(original.mSizeLineal),
//    mColorGradient(original.mColorGradient), Component(owner, ComponentType::PARTICLESYSTEM)
//{
//    SetImage(original.mResourceId);
//    Init();
//}
//
//TrailComponent::~TrailComponent()
//{
//    //App->GetOpenGL()->RemoveTrail(mTrail);
//}
//
//Component* TrailComponent::Clone(GameObject* owner) const
//{
//    return new TrailComponent(*this, owner);
//}
//
//
//
//void TrailComponent::Init()
//{
//
//}
//
//void TrailComponent::Update()
//{
//    float3 position = mOwner->GetPosition();
//    float dposition = position.DistanceSq(mTrail.GetLastPosition());
//    if (dposition >= mMinDistance * mMinDistance && mTrail.GetSize() < mMaxPoints)
//    {
//        mTrail.AddTrailPositions(position, mOwner->GetRotation());
//    }
//    mTrail.Update();
//
//}
//
//void TrailComponent::SetImage(unsigned int resourceId)
//{
//    mResourceId = resourceId;
//    ResourceTexture* image = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
//    mTrail.SetImage(image);
//}
//
//void TrailComponent::Reset()
//{
//
//}
//
//void TrailComponent::Save(Archive& archive) const
//{
//    Component::Save(archive);
//    archive.AddInt("Image", mResourceId);
//}
//
//void TrailComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
//{
//    Component::LoadFromJSON(data, owner);
//    if (data.HasMember("Duration") && data["Duration"].IsFloat())
//    {
//        //mDuration = data["Duration"].GetFloat();
//    }
//    if (data.HasMember("Image") && data["Image"].IsInt())
//    {
//        mResourceId = data["Image"].GetInt();
//        SetImage(mResourceId);
//    }
//    if (data.HasMember("Life Time") && data["Life Time"].IsFloat())
//    {
//        mMaxLifeTime = data["Life Time"].GetFloat();
//    }
//}
//
