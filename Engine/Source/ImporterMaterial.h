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
		ResourceMaterial* Import(const unsigned int uid, const tinygltf::Model& model, const tinygltf::Material& material);
        void Save(const ResourceMaterial* ourMaterial);
		ResourceMaterial* Load(const unsigned int uid, const char* fileName = nullptr);
	}
};

