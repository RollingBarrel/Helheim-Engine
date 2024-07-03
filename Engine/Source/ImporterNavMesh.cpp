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
#include "SaveLoadNavMesh.h"


ResourceNavMesh* Importer::NavMesh::Import(unsigned int uid,const char* assetsFile)
{
	dtNavMesh* detourNavMesh = App->GetNavigation()->GetDetourNavMesh();
	
	if (!detourNavMesh) 
	{
		CheckLibraryFileExists(uid, assetsFile);
		return nullptr;
	}

	ResourceNavMesh* resourceNavMesh = new ResourceNavMesh(uid, detourNavMesh);

	std::string navMeshName = App->GetScene()->GetRoot()->GetName();

	Importer::NavMesh::Save(resourceNavMesh, navMeshName.c_str());

	std::string pathStr = std::string(ASSETS_NAVMESH_PATH);

	EngineApp->GetEngineResource()->CreateAssetsMeta(*resourceNavMesh, (pathStr + navMeshName + ".navmesshi").c_str());

	return resourceNavMesh;
}


void Importer::NavMesh::CheckLibraryFileExists(unsigned int uid, const char* assetsFile)
{
	const char* libraryFile = EngineApp->GetFileSystem()->GetLibraryFile(uid);

	if(!EngineApp->GetFileSystem()->Exists(libraryFile))
	{
		char* fileBuffer = nullptr;
		int size = App->GetFileSystem()->Load(assetsFile, &fileBuffer);
		const char* path = App->GetFileSystem()->GetLibraryFile(uid, true);
		App->GetFileSystem()->Save(path, fileBuffer, size);
		delete[] path;
		delete[] fileBuffer;
	}
	delete[] libraryFile;
}










