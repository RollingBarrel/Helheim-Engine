#include <string>
#include <iostream>
#include <fstream>

#include "SaveLoadScript.h"
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