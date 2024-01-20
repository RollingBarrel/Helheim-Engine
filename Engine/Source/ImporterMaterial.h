#pragma once

#include "MathGeoLibFwd.h"

#include "float3.h"
#include "float4.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

struct ResourceTexture;


struct ResourceMaterial
{
public:
    ResourceTexture* GetDiffuseMap() const { return mDiffuseTexture; };
    ResourceTexture* GetSpecularMap()const { return mSpecularGlossinessTexture; };
    ResourceTexture* GetNormalMap()const { return mNormalTexture; };
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

    int mTemporalID = -1;
};

namespace Importer
{
	namespace Material
	{
		void Import(const tinygltf::Model& model, const tinygltf::Material& material, ResourceMaterial* rMaterial);
	}
};

