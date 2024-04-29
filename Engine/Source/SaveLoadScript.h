#pragma once
#include "Globals.h"

class ResourceScript;

namespace Importer
{
	namespace Script
	{
		ENGINE_API void Save(const ResourceScript* ourTexture);
		ENGINE_API ResourceScript* Load(const char* filePath, unsigned int uid);
	}
}

