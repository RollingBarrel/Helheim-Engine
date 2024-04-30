#pragma once
#include "Globals.h"

struct ResourceNavMesh;

namespace Importer
{
    namespace NavMesh
    {
        ENGINE_API void Save(const ResourceNavMesh& navMesh);
        ENGINE_API void Load(ResourceNavMesh& navMesh, const char* fileName = nullptr);
    }
};