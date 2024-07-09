#pragma once
#include "Globals.h"

class ResourceNavMesh;

namespace Importer
{
    namespace NavMesh
    {
        ENGINE_API void Save(ResourceNavMesh* navMesh);
        ENGINE_API  ResourceNavMesh* Load(const char* fileName, unsigned int uid);
    }
};