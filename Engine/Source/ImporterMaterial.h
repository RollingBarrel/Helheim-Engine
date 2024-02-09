#pragma once

#include "MathGeoLibFwd.h"

#include "float3.h"
#include "float4.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

class ResourceMaterial;

namespace Importer
{
	namespace Material
	{
		void Import(const tinygltf::Model& model, const tinygltf::Material& material, ResourceMaterial* rMaterial);
        void Save(const ResourceMaterial* ourMaterial);
        void Load(ResourceMaterial* ourMaterial, const char* fileName = nullptr);
	}
};

