#pragma once
#include "Globals.h"
class ResourceMaterial;

namespace Importer
{
	namespace Material
	{;
		ENGINE_API void Save(const ResourceMaterial* ourMaterial);
		ENGINE_API ResourceMaterial* Load(const char* fileName, const unsigned int uid);
		ENGINE_API void SaveMatFile(const ResourceMaterial& rMat, const char* name = nullptr);
	}
};