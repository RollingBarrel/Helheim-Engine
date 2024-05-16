#include "ImporterNavMesh.h"
#include <vector>
#include "ResourceNavMesh.h"
#include "EngineApp.h"
#include "ModuleEngineResource.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
#include "ModuleScene.h"
#include "NavMeshController.h"
#include "GameObject.h"
#include "Recast.h"
#include "Resource.h"
#include "DetourNavMesh.h"
#include "ModuleDetourNavigation.h"


ResourceNavMesh* Importer::NavMesh::Import(unsigned int uid)
{
	dtNavMesh* detourNavMesh = App->GetNavigation()->GetDetourNavMesh();

	ResourceNavMesh* resourceNavMesh = new ResourceNavMesh(uid, detourNavMesh);

	std::string navMeshName = App->GetScene()->GetRoot()->GetName();

	Importer::NavMesh::Save(resourceNavMesh, navMeshName);

	std::string pathStr = std::string(ASSETS_NAVMESH_PATH);

	EngineApp->GetEngineResource()->CreateAssetsMeta(*resourceNavMesh, (pathStr + navMeshName + ".navmesshi").c_str());

	return resourceNavMesh;
}










