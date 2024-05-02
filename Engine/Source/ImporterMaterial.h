#pragma once
#include <map>

namespace tinygltf {
	class Model;
	struct Material;
}

class ResourceMaterial;

namespace Importer
{
	namespace Material
	{
		ResourceMaterial* Import(const char* path, const tinygltf::Model& model, const tinygltf::Material& material, unsigned int& uid, std::map<unsigned int, unsigned int>& importedTextures, bool modifyAssets = true);
		ResourceMaterial* ImportDefault();
	}
}

