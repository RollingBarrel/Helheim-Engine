#pragma once

class ResourceNavMesh;

namespace Importer
{
    namespace NavMesh
    {
        ResourceNavMesh* Import(unsigned int uid, const char* assetsFile);
        void CheckLibraryFileExists(unsigned int uid, const char* assetsFile);
    }
};