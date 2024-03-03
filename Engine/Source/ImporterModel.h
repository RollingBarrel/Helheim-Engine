#pragma once
#include <vector>

#include "tiny_gltf.h"

class ResourceModel;
class ModelNode;

namespace Importer
{
	namespace Model
	{
		ResourceModel* Import(const char* filePath, unsigned int uid, bool modifyAssets = true);

		void Save(const ResourceModel* ourModel);

		ResourceModel* Load(const char* fileName, unsigned int uid);

		ModelNode& ImportNode(const char* filePath, const tinygltf::Model& model, unsigned int index, unsigned int uid, bool modifyAssets = true);
	};
};

