#pragma once
#include <vector>

#include "Resource.h"

class ResourceModel;

namespace Importer
{
	namespace Model
	{
		void Import(const char* filePath, ResourceModel* rModel);

		void Save(const ResourceModel* ourModel);

		void Load(ResourceModel* ourModel, const char* fileName);
	}
};

