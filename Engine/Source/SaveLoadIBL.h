#pragma once
#include "Globals.h"

class ResourceIBL;

namespace Importer
{
	namespace IBL
	{
		ENGINE_API void Save(const ResourceIBL* resource);
		ENGINE_API ResourceIBL* Load(const char* filePath, unsigned int uid);
	}
}