#pragma once
#include <vector>

#include "Resource.h"

class ResourceModel;

namespace Importer
{
	namespace Model
	{
		ResourceModel* Import(const char* filePath, unsigned int uid);

		void Save(const ResourceModel* ourModel);

		ResourceModel* Load(const char* fileName, unsigned int uid);
	}
};

