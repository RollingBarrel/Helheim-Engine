#pragma once

#include "Globals.h"
class ResourceMesh;

namespace Importer
{
	namespace Mesh
	{
		ENGINE_API void Save(const ResourceMesh* ourMesh);

		ENGINE_API ResourceMesh* Load(const char* fileName, unsigned int uid);
	}
}