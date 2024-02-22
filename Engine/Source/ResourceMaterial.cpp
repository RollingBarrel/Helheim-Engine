#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleResource.h"

ResourceMaterial::ResourceMaterial(
    unsigned int uid,
    const char* path,
    float4 diffuseFactor,
    float3 specularFactor,
    float glossinessFactor,
    unsigned int diffuseTextureUid,
    unsigned int specularGlossinessTextureUid,
    unsigned int normalTextureUid) :
    Resource(uid, Type::Material, path, ".mat"),
    mDiffuseFactor(diffuseFactor), mSpecularFactor(specularFactor), mGlossinessFactor(glossinessFactor),
    mDiffuseTexture(reinterpret_cast<ResourceTexture*>(App->GetResource()->RequestResource(diffuseTextureUid))),
    mSpecularGlossinessTexture(reinterpret_cast<ResourceTexture*>(App->GetResource()->RequestResource(specularGlossinessTextureUid))),
    mNormalTexture(reinterpret_cast<ResourceTexture*>(App->GetResource()->RequestResource(normalTextureUid)))
{
    
    //if (mDiffuseTexture) mEnableDiffuseTexture = true;
    //else mEnableDiffuseTexture = false;
    mEnableDiffuseTexture = (mDiffuseTexture != nullptr) ? true : false;

    if (mSpecularGlossinessTexture) {
        mEnableSpecularGlossinessTexture = true;
        if (mSpecularGlossinessTexture->HasAlpha())
            mEnableShininessMap = true;
        else mEnableShininessMap = false;
    }
    else {
        mEnableSpecularGlossinessTexture = false;
        mEnableShininessMap = false;
    }

    //if (mNormalTexture) mEnableNormalMap = true;
    //else mEnableNormalMap = false;
    mEnableNormalMap = (mNormalTexture != nullptr) ? true : false;
}

ResourceMaterial::~ResourceMaterial()
{ 
    if (mDiffuseTexture)
        App->GetResource()->ReleaseResource(mDiffuseTexture->GetUID());
    if(mSpecularGlossinessTexture)
        App->GetResource()->ReleaseResource(mSpecularGlossinessTexture->GetUID());
    if(mNormalTexture)
        App->GetResource()->ReleaseResource(mNormalTexture->GetUID());
}