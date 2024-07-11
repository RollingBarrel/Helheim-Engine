#pragma once
#include "Resource.h"
class dtNavMesh;

class ENGINE_API ResourceNavMesh : public Resource
{
public:
    ResourceNavMesh(unsigned int uid, dtNavMesh* navMesh);
    ~ResourceNavMesh();
    dtNavMesh* GetDtNavMesh() { return mDtNavMesh; }

private:

    dtNavMesh* mDtNavMesh = nullptr;

};