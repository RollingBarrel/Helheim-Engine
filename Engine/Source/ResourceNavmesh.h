#pragma once
#include "Resource.h"
#include "vector"
#include "float3.h"

class ResourceNavmesh :
    public Resource
{
public:
    ResourceNavmesh(unsigned int uid, std::vector<float3> vertices, std::vector<int> indices);
    ~ResourceNavmesh();

    std::vector<float3> GetVertices() const { return mVertices; }
    std::vector<int> GetIndices() const { return mIndices; }


private:
    std::vector<float3> mVertices;
    std::vector<int> mIndices;
};

