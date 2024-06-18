#pragma once

#include "Globals.h"
class ResourceScene;

namespace Importer
{
	namespace Scene
	{
		ENGINE_API void Save(const ResourceScene* ourScene, const char* assetsFile);

		ENGINE_API ResourceScene* Load(const char* fileName, unsigned int uid);	//Redundant fileName??
	}
}