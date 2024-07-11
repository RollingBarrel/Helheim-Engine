#pragma once
#include "Resource.h"
class dtNavMesh;

class ENGINE_API ResourceNavMesh : public Resource
{
public:
    ResourceNavMesh(unsigned int uid, dtNavMesh* navMesh);
    ~ResourceNavMesh();
    const dtNavMesh* GetDtNavMesh() const { return mDtNavMesh; }

private:

    dtNavMesh* mDtNavMesh = nullptr;

};