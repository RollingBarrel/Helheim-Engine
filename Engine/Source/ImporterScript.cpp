#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterScript.h"
#include "ResourceScript.h"

// Function to parse a header file and extract attribute names and types
bool parseHeaderFile(const std::string& filename, ResourceScript* rScript)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return false;
    }

    return true;
}

ResourceScript* Importer::Script::Import(const char* filePath, unsigned int uid)
{
    std::string filename = (filePath);
    ResourceScript* rScript = new ResourceScript(uid);
    parseHeaderFile(filename, rScript);
    Importer::Script::Save(rScript);
    return rScript;
}

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



