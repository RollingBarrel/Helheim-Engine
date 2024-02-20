#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "ModuleFileSystem.h"

ResourceMaterial::ResourceMaterial()
{
}

ResourceMaterial::ResourceMaterial(unsigned int uid) : Resource(uid)
{
    mType = Type::Material;
    mAssetsFile = "NONE";
    mLibraryFile = LIBRARY_MATERIAL_PATH + std::to_string(mUID) + ".mat";
}

ResourceMaterial::ResourceMaterial(
    unsigned int uid,
    float4 diffuseFactor, 
    float3 specularFactor, 
    float glossinessFactor,
    ResourceTexture* diffuseTexture, 
    ResourceTexture* specularGlossinessTexture, 
    ResourceTexture* normalTexture) :
    Resource(uid),
    mDiffuseFactor(diffuseFactor), mSpecularFactor(specularFactor), mGlossinessFactor(glossinessFactor),
    mDiffuseTexture(diffuseTexture), mSpecularGlossinessTexture(specularGlossinessTexture), mNormalTexture(normalTexture)
{
    
    //if (mDiffuseTexture) mEnableDiffuseTexture = true;
    //else mEnableDiffuseTexture = false;
    mEnableDiffuseTexture = (!mDiffuseTexture) ? true : false;

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
    mEnableNormalMap = (!mNormalTexture) ? true : false;

    mType = Type::Material;

    mLibraryFile = LIBRARY_MATERIAL_PATH + std::to_string(mUID) + ".mat";
}