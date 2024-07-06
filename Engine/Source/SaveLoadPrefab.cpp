#include "SaveLoadPrefab.h"
#include "ResourcePrefab.h"
#include "Application.h"
#include "ModuleFileSystem.h"


void Importer::Prefab::Save(const ResourcePrefab* ourPrefab, const char* assetsFile)
{
    char* fileBuffer;
    unsigned int size = App->GetFileSystem()->Load(assetsFile, &fileBuffer); //Do it in the Import?

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(ourPrefab->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);
    delete[] fileBuffer;
    delete[] libraryPath;
}

ResourcePrefab* Importer::Prefab::Load(const char* fileName, unsigned int uid)
{
    ResourcePrefab* rPrefab = new ResourcePrefab(uid);
	return rPrefab;
}
