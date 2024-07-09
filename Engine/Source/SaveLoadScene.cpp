#include "SaveLoadScene.h"
#include "ResourceScene.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleScene.h"

void Importer::Scene::Save(const ResourceScene* ourScene, const char* assetsFile)
{
    char* fileBuffer;
    unsigned int size = App->GetFileSystem()->Load(assetsFile, &fileBuffer); //Do it in the Import?

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(ourScene->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);
    delete[] fileBuffer;
    delete[] libraryPath;
}

ResourceScene* Importer::Scene::Load(const char* fileName, unsigned int uid)
{
    char* fileBuffer = nullptr;

    if (App->GetFileSystem()->Load(fileName, &fileBuffer) > 0)
    {
        App->GetScene()->Load(fileBuffer);
    }

    delete[] fileBuffer;

    return nullptr;
}
