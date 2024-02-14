#pragma once
struct ResourceNavMesh
{
public:

};

namespace Importer
{
    namespace NavMesh
    {
        void Import();
        void Save(const ResourceNavMesh* ourNavMesh);
        void Load(ResourceNavMesh* ourNavMesh , const char* fileName = nullptr);
    }
};