#include "ResourceNavmesh.h"

ResourceNavmesh::ResourceNavmesh(unsigned int uid, std::vector<float3> vertices, std::vector<int> indices) : Resource(uid, Type::NavMesh), mVertices(vertices), mIndices(indices)
{
}

ResourceNavmesh::~ResourceNavmesh()
{
	mVertices.clear();
	mIndices.clear();
}

