#pragma once

struct ResourceNavMesh;

namespace Importer
{
    namespace NavMesh
    {
        void Save(const ResourceNavMesh& navMesh);
        void Load(ResourceNavMesh& navMesh, const char* fileName = nullptr);
    }
};