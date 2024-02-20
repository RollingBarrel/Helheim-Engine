#pragma once
#include "Resource.h"

#include "float3.h"
#include "float4.h"

class ResourceTexture;

class ResourceMaterial : public Resource
{
public:
    ResourceMaterial();
	ResourceMaterial(unsigned int uid);
    ResourceMaterial(
        unsigned int uid, 
        float4 diffuseFactor, 
        float3 specularFactor, 
        float glossinessFactor, 
        ResourceTexture* diffuseTexture, 
        ResourceTexture* specularGlossinessTexture, 
        ResourceTexture* normalTexture);

    ~ResourceMaterial() { delete mDiffuseTexture; delete mSpecularGlossinessTexture; delete mNormalTexture; }

    // Getter functions
    float4 GetDiffuseFactor() const { return mDiffuseFactor; }
    float3 GetSpecularFactor() const { return mSpecularFactor; }
    float GetGlossinessFactor() const { return mGlossinessFactor; }
    ResourceTexture* GetDiffuseTexture() const { return mDiffuseTexture; }
    ResourceTexture* GetSpecularGlossinessTexture() const { return mSpecularGlossinessTexture; }
    ResourceTexture* GetNormalTexture() const { return mNormalTexture; }
    bool IsDiffuseTextureEnabled() const { return mEnableDiffuseTexture; }
    bool IsSpecularGlossinessTextureEnabled() const { return mEnableSpecularGlossinessTexture; }
    bool IsNormalMapEnabled() const { return mEnableNormalMap; }
    bool IsShinessMapEnabled() const { return mEnableShinessMap; }

private:
    float4 mDiffuseFactor;
    float3 mSpecularFactor;
    float mGlossinessFactor;

    ResourceTexture* mDiffuseTexture = nullptr;
    ResourceTexture* mSpecularGlossinessTexture = nullptr;
    ResourceTexture* mNormalTexture = nullptr;

    bool mEnableDiffuseTexture;
    bool mEnableSpecularGlossinessTexture;
    bool mEnableNormalMap;
    bool mEnableShinessMap;
};

