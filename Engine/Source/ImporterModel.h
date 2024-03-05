#pragma once
#include <vector>

class ResourceModel;

namespace Importer
{
	namespace Model
	{
		ResourceModel* Import(const char* filePath, unsigned int uid, bool modifyAssets = true);

		void Save(const ResourceModel* ourModel, unsigned int& size);

		ResourceModel* Load(const char* fileName, unsigned int uid);

	};
};

