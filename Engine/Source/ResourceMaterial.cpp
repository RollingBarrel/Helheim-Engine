#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "ModuleFileSystem.h"

ResourceMaterial::ResourceMaterial(
    unsigned int uid,
    const char* path,
    float4 diffuseFactor, 
    float3 specularFactor, 
    float glossinessFactor,
    ResourceTexture* diffuseTexture, 
    ResourceTexture* specularGlossinessTexture, 
    ResourceTexture* normalTexture) :
    Resource(uid, Type::Material, path, ".mat"),
    mDiffuseFactor(diffuseFactor), mSpecularFactor(specularFactor), mGlossinessFactor(glossinessFactor),
    mDiffuseTexture(diffuseTexture), mSpecularGlossinessTexture(specularGlossinessTexture), mNormalTexture(normalTexture)
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
    if(mDiffuseTexture)
        delete mDiffuseTexture; 
    if(mSpecularGlossinessTexture)
        delete mSpecularGlossinessTexture;
    if(mNormalTexture)
        delete mNormalTexture; 
}