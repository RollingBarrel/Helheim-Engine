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

    bool GetEnableDiffuseTexture()const { return mEnableDiffuseTexture; };
    bool GetEnableSpecularGlossinessTexture()const { return mEnableSpecularGlossinessTexture; };
    bool GetEnableShinessMap()const { return mEnableShinessMap; };

    void SetDiffuseFactor(const float4& diffuseFactor) { mDiffuseFactor = diffuseFactor; }
    void SetSpecularFactor(const float3& specularFactor) { mSpecularFactor = specularFactor; }
    void SetGlossinessFactor(float glossinessFactor) { mGlossinessFactor = glossinessFactor; }

    void SetEnableDiffuseTexture(bool enableDiffuseTexture) { mEnableDiffuseTexture = enableDiffuseTexture; }
    void SetEnableSpecularGlossinessTexture(bool enableSpecularGlossinessTexture) { mEnableSpecularGlossinessTexture = enableSpecularGlossinessTexture; }
    void SetEnableShinessMap(bool enableShinessMap) { mEnableShinessMap = enableShinessMap; }

private:

    float4 mDiffuseFactor;
    float3 mSpecularFactor;
    float mGlossinessFactor;

    Texture* mDiffuseTexture = nullptr;
    Texture* mSpecularGlossinessTexture = nullptr;

    bool mEnableDiffuseTexture;
    bool mEnableSpecularGlossinessTexture;
    bool mEnableShinessMap;
};
