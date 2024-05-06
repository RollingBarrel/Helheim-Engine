#include "ResourceNavMesh.h"
#include "Recast.h"
#include "DetourNavMesh.h"
ResourceNavMesh::ResourceNavMesh(unsigned int uid, dtNavMesh* navmesh)
	: Resource(uid, Type::NavMesh),mDtNavMesh(navmesh)

{
	
}

ResourceNavMesh::~ResourceNavMesh()
{
}

