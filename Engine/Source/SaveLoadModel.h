#pragma once
#include "Globals.h"
class ResourceModel;

namespace Importer
{
	namespace Model
	{
		ENGINE_API void Save(const ResourceModel* ourModel, unsigned int& size);
		ENGINE_API ResourceModel* Load(const char* fileName, unsigned int uid);
	};
};