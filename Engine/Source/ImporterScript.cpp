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
    std::string line;
    std::string scriptName;
    while (scriptName == "" and std::getline(file, line))
    {
        size_t classPos = line.find("class ");
        if (classPos != std::string::npos) {
            // Find the position of the first space after "class "
            classPos += 6;
            size_t spacePos = line.find_first_of(" ", classPos);
            if (spacePos != std::string::npos) {
                // Extract the word after "class "
                scriptName = line.substr(classPos, spacePos - classPos);
            }
            else
            {
                scriptName = line.substr(classPos);
            }
        }
    }
    rScript->SetScriptName(scriptName);

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
    size_t sizeScriptName = rScript->GetScriptName().length();

    // size of all the data to be saved
    size_t size = sizeof(size_t) +
        sizeScriptName * sizeof(char); 
    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    size_t bytes = sizeof(size_t);
    memcpy(cursor, &sizeScriptName, bytes);
    cursor += bytes;

    bytes = sizeScriptName * sizeof(char);
    memcpy(cursor, rScript->GetScriptName().c_str(), bytes);
    cursor += bytes;

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

        size_t sizeTBytes = sizeof(size_t);

        size_t sizeScriptName;
        memcpy( &sizeScriptName, cursor, sizeTBytes);
        cursor += sizeTBytes;

        std::string scriptName;
        memcpy(&scriptName, cursor, sizeScriptName);
        cursor += sizeScriptName;
        rScript->SetScriptName(scriptName);

        delete[] fileBuffer;
    }
    
    return rScript;
}



