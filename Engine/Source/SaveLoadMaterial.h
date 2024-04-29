#pragma once
#include "Globals.h"
class ResourceMaterial;

namespace Importer
{
	namespace Material
	{;
		ENGINE_API void Save(const ResourceMaterial* ourMaterial);
		ENGINE_API ResourceMaterial* Load(const char* fileName, const unsigned int uid);
	}
};