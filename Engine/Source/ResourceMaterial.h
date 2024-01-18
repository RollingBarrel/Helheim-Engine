#pragma once
#include "Math/float3.h"
#include "Math/float4.h"
#include "tiny_gltf.h"
class Texture;
class ResourceMaterial
{
public:
    ResourceMaterial();
    ~ResourceMaterial();

    void LoadMaterial(const tinygltf::Model& model, const tinygltf::Material& material);
    Texture* GetDiffuseMap() const { return mDiffuseTexture; };
    Texture* GetSpecularMap()const { return mSpecularGlossinessTexture; };
    float4 GetDiffuseFactor()const { return mDiffuseFactor; };
    float3 GetSpecularFactor()const { return mSpecularFactor; };
    float GetGlossinessFactor()const { return mGlossinessFactor; };

private:

    float4 mDiffuseFactor;
    float3 mSpecularFactor;
    float mGlossinessFactor;

    Texture* mDiffuseTexture = nullptr;
    Texture* mSpecularGlossinessTexture = nullptr;
};
