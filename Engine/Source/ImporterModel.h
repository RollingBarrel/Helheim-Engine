#pragma once
#include <vector>

#include "Resource.h"

class ResourceModel;

namespace Importer
{
	namespace Model
	{
		ResourceModel* Import(const char* filePath);

		void Save(const ResourceModel* ourModel);

		void Load(ResourceModel* ourModel, const char* fileName);
	}
};

