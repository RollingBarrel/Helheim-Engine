#pragma once
#include "MathGeoLibFwd.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

class ResourceMaterial;

namespace Importer
{
	namespace Material
	{
		ResourceMaterial* Import(const char* path, const tinygltf::Model& model, const tinygltf::Material& material, unsigned int& uid, bool modifyAssets = true);
		ResourceMaterial* ImportDefault();
		void Save(const ResourceMaterial* ourMaterial);
		ResourceMaterial* Load(const char* fileName, const unsigned int uid);
	}
};

