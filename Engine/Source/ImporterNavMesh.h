#pragma once
#include <string>

class ResourceNavMesh;

namespace Importer
{
    namespace NavMesh
    {
        ResourceNavMesh* Import(unsigned int uid);
        void Save( ResourceNavMesh* navMesh, std::string name);

        ResourceNavMesh* Load(const char* fileName, unsigned int uid);
    }
};