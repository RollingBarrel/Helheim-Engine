#pragma once
#include "Globals.h"

class ResourcePrefab;

namespace Importer
{
    namespace Prefab
    {
        ENGINE_API void Save(const ResourcePrefab* navMesh, const char* fileName);
        ENGINE_API  ResourcePrefab* Load(const char* fileName, unsigned int uid);
    }
};