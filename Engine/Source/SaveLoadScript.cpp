#include "SaveLoadScript.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "ResourceScript.h"

void Importer::Script::Save(const ResourceScript* rScript)
{
    // size of all the data to be saved
    size_t size = 0; 
    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rScript->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceScript* Importer::Script::Load(const char* filePath, unsigned int uid)
{
    char* fileBuffer;
    ResourceScript* rScript = new ResourceScript(uid);

    if (App->GetFileSystem()->Load(filePath, &fileBuffer))
    {
       char* cursor = fileBuffer;

        delete[] fileBuffer;
    }
    
    return rScript;
}



