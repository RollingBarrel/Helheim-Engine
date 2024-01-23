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
    ~ResourceMaterial() { delete mDiffuseTexture; delete mSpecularGlossinessTexture; delete mNormalTexture;  }

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

    unsigned int mUID;
};

namespace Importer
{
	namespace Material
	{
		void Import(const tinygltf::Model& model, const tinygltf::Material& material, ResourceMaterial* rMaterial);
        void Save(const ResourceMaterial* ourMaterial);
        void Load(ResourceMaterial* ourMaterial, const char* fileName = nullptr);
	}
};

