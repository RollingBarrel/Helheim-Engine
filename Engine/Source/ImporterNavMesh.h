#pragma once

class ResourceNavMesh;
class dtNavMesh;

namespace Importer
{
    namespace NavMesh
    {
        ResourceNavMesh* Import(unsigned int uid, const char* assetsFile);
        void SaveAsset(const char* path, const dtNavMesh& mesh);
    }
};