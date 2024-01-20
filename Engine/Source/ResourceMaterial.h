#pragma once
#include "Math/float3.h"
#include "Math/float4.h"

class Texture;

namespace tinygltf {
    class Model;
    class Material;
};

class ResourceMaterial
{
public:
    ResourceMaterial();
    ~ResourceMaterial();

    void LoadMaterial(const tinygltf::Model& model, const tinygltf::Material& material);
    Texture* GetDiffuseMap() const { return mDiffuseTexture; };
    Texture* GetSpecularMap()const { return mSpecularGlossinessTexture; };
    Texture* GetNormalMap()const { return mNormalTexture; };
    float4 GetDiffuseFactor()const { return mDiffuseFactor; };
    float3 GetSpecularFactor()const { return mSpecularFactor; };
    float GetGlossinessFactor()const { return mGlossinessFactor; };

    bool GetEnableDiffuseTexture()const { return mEnableDiffuseTexture; };
    bool GetEnableSpecularGlossinessTexture()const { return mEnableSpecularGlossinessTexture; };
    bool GetEnableShinessMap()const { return mEnableShinessMap; };
    bool GetEnableNormalMap()const { return mEnableNormalMap; };

    void SetDiffuseFactor(const float4& diffuseFactor) { mDiffuseFactor = diffuseFactor; }
    void SetSpecularFactor(const float3& specularFactor) { mSpecularFactor = specularFactor; }
    void SetGlossinessFactor(float glossinessFactor) { mGlossinessFactor = glossinessFactor; }

    void SetEnableDiffuseTexture(bool enableDiffuseTexture) { mEnableDiffuseTexture = enableDiffuseTexture; }
    void SetEnableSpecularGlossinessTexture(bool enableSpecularGlossinessTexture) { mEnableSpecularGlossinessTexture = enableSpecularGlossinessTexture; }
    void SetEnableShinessMap(bool enableShinessMap) { mEnableShinessMap = enableShinessMap; }
    void SetTemporalID(int id) { mTemporalID = id; }

private:

    float4 mDiffuseFactor;
    float3 mSpecularFactor;
    float mGlossinessFactor;

    Texture* mDiffuseTexture = nullptr;
    Texture* mSpecularGlossinessTexture = nullptr;
    Texture* mNormalTexture = nullptr;

    bool mEnableDiffuseTexture;
    bool mEnableSpecularGlossinessTexture;
    bool mEnableNormalMap;
    bool mEnableShinessMap;

    int mTemporalID = -1;
};
