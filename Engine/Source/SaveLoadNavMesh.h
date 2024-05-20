#pragma once
#include "Globals.h"
#include <string>
class ResourceNavMesh;

namespace Importer
{
    namespace NavMesh
    {
        ENGINE_API void Save(ResourceNavMesh* navMesh, std::string name);
        ENGINE_API  ResourceNavMesh* Load(const char* fileName, unsigned int uid);
        

       
    }
};