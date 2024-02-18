#include "ResourceMaterial.h"
#include "ResourceTexture.h"

ResourceMaterial::ResourceMaterial()
{
}

ResourceMaterial::ResourceMaterial(unsigned int uid) : Resource(uid)
{
}

ResourceMaterial::ResourceMaterial(
    unsigned int uid,
    float4 diffuseFactor, float3 specularFactor, float glossinessFactor,
    ResourceTexture* diffuseTexture, ResourceTexture* specularGlossinessTexture, ResourceTexture* normalTexture) :
    Resource(uid),
    mDiffuseFactor(diffuseFactor), mSpecularFactor(specularFactor), mGlossinessFactor(glossinessFactor),
    mDiffuseTexture(diffuseTexture), mSpecularGlossinessTexture(specularGlossinessTexture), mNormalTexture(normalTexture)
{
    if (mDiffuseTexture) mEnableDiffuseTexture = true;
    else mEnableDiffuseTexture = false;

    if (mSpecularGlossinessTexture) {
        mEnableSpecularGlossinessTexture = true;
        if (mSpecularGlossinessTexture->HasAlpha())
            mEnableShinessMap = true;
        else mEnableShinessMap = false;
    }
    else {
        mEnableSpecularGlossinessTexture = false;
        mEnableShinessMap = false;
    }

    if (mNormalTexture) mEnableNormalMap = true;
    else mEnableNormalMap = false;

}