#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleResource.h"

ResourceMaterial::ResourceMaterial(
    unsigned int uid,
    float* baseColorFactor,
    float metallicFactor,
    float roughnessFactor,
    unsigned int baseColorTexUid,
    unsigned int metallicRoughTexUid,
    unsigned int normalTexUid) :
    Resource(uid, Type::Material),
    mBaseColorFactor(float4(baseColorFactor)), mMetallicFactor(metallicFactor), mRoughnessFactor(roughnessFactor),
    mBaseColorTexture((baseColorTexUid != 0) ? reinterpret_cast<ResourceTexture*>(App->GetResource()->RequestResource(baseColorTexUid, Resource::Type::Texture)) : nullptr),
    mMetallicRoughnessTexture((metallicRoughTexUid != 0) ? reinterpret_cast<ResourceTexture*>(App->GetResource()->RequestResource(metallicRoughTexUid, Resource::Type::Texture)) : nullptr),
    mNormalTexture((normalTexUid != 0) ? reinterpret_cast<ResourceTexture*>(App->GetResource()->RequestResource(normalTexUid, Resource::Type::Texture)) : nullptr),
    mEnableBaseColorTexture((mBaseColorTexture != nullptr) ? true : false),
    mEnableMetallicRoughnessTexture((mMetallicRoughnessTexture != nullptr) ? true : false),
    mEnableNormalMap((mNormalTexture != nullptr) ? true : false)
{
}

ResourceMaterial::~ResourceMaterial()
{ 
    if (mBaseColorTexture)
        App->GetResource()->ReleaseResource(mBaseColorTexture->GetUID());
    if(mMetallicRoughnessTexture)
        App->GetResource()->ReleaseResource(mMetallicRoughnessTexture->GetUID());
    if(mNormalTexture)
        App->GetResource()->ReleaseResource(mNormalTexture->GetUID());
}