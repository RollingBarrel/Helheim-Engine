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
    ~ResourceMaterial() { delete mDiffuseTexture; delete mSpecularGlossinessTexture; delete mNormalTexture; }

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

