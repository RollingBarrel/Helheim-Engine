#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterScript.h"
#include "ResourceScript.h"

std::string trim(const std::string& str, const char* errasable) {
    size_t first = str.find_first_not_of(errasable);
    if (std::string::npos == first) {
        return str;
    }
    size_t last = str.find_last_not_of(errasable);
    return str.substr(first, (last - first + 1));
}

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

    // Init a ScriptAttribute
    ScriptAttribute attribute = ScriptAttribute();
    bool serializable = true;

    // Read line by line in the .h file
    while (std::getline(file, line))
    {
        std::string trimmedLine = trim(line, " \t"); // get rid of white spaces
        std::string firstWord;
        size_t firstSpacePos = trimmedLine.find(' ');
        if (firstSpacePos != std::string::npos)
        {
            firstWord = trimmedLine.substr(0, firstSpacePos);
        }
        else {
            firstWord = trimmedLine;
        }
     
        // if this line has a declaration of a variable
        if (trimmedLine.find(';') != std::string::npos and trimmedLine.find('(') == std::string::npos)
        {
            trimmedLine = trimmedLine.substr(0, trimmedLine.find(';'));
            std::istringstream iss(trimmedLine);

            // Extract the variable type
            std::string type;
            if (!(iss >> type))
            {
                // se esperaba el tipo de la variable
                return false;
            }
            // when there is a keyword before the type extract the type again
            if (type == "const")
            {
                if (!(iss >> type))
                {
                    // se esperaba el tipo de la variable
                    return false;
                }
            }
            // try to convert the 'type' to a ScriptAttributeType and set it in the ScriptAttribute
            if (!attribute.setTypeFromString(type))
            {
                // el tipo de la variable no es valido
                return false;
            }
            // Extract the variable name
            std::string name;
            if (!(iss >> name))
            {
                // se esperaba el nombre de la variable
                return false;
            }
            attribute.setName(name.c_str());

            if (serializable)
            {
                rScript->addAttribute(attribute);
            }
            attribute = ScriptAttribute();
            bool serializable = true;
        }
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
    size_t sizeScriptName = rScript->GetScriptName().length();
    size_t sizeAttributes = rScript->GetAttributes().size();
    size_t sizeScriptAttributes = 0;
    for (size_t i = 0; i < sizeAttributes; ++i)
    {
        sizeScriptAttributes += rScript->GetAttributes()[i].sizeOfScriptVariable();
    }

    // size of all the data to be saved
    size_t size = sizeof(size_t) * 3 +
        sizeScriptName * sizeof(char) +
        sizeScriptAttributes;
    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    size_t bytes = sizeof(size_t);
    memcpy(cursor, &sizeScriptName, bytes);
    cursor += bytes;
    memcpy(cursor, &sizeAttributes, bytes);
    cursor += bytes;

    bytes = sizeScriptName * sizeof(char);
    memcpy(cursor, rScript->GetScriptName().c_str(), bytes);
    cursor += bytes;

    ScriptAttribute attribute;
    for (size_t i = 0; i < sizeAttributes; ++i)
    {
        attribute = rScript->GetAttributes()[i];
        size_t nameSizeInBytes = attribute.GetName().size() * sizeof(char);
        size_t sizeTSizeInBites = sizeof(size_t);
        memcpy(cursor, &nameSizeInBytes, sizeTSizeInBites);
        cursor += sizeTSizeInBites;
        memcpy(cursor, attribute.GetName().c_str(), nameSizeInBytes);
        cursor += nameSizeInBytes;
        size_t typeSizeInBytes = sizeof(MemberType);
        MemberType type = attribute.GetType();
        memcpy(cursor, &type, typeSizeInBytes);
        cursor += typeSizeInBytes;
    }

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
       /* char* cursor = fileBuffer;

        size_t sizeTBytes = sizeof(size_t);

        size_t sizeScriptName;
        memcpy( &sizeScriptName, cursor, sizeTBytes);
        cursor += sizeTBytes;

        size_t sizeAttributes;
        memcpy( &sizeAttributes, cursor, sizeTBytes);
        cursor += sizeTBytes;

        std::string scriptName;
        memcpy(&scriptName, cursor, sizeScriptName);
        cursor += sizeScriptName;
        rScript->SetScriptName(scriptName);

        for (size_t i = 0; i < sizeAttributes; ++i)
        {
            ScriptAttribute attribute;

            size_t nameSizeInBytes;
            size_t sizeTSizeInBites = sizeof(size_t);
            memcpy(&nameSizeInBytes, cursor, sizeTSizeInBites);
            cursor += sizeTSizeInBites;
            char* name = new char[nameSizeInBytes];
            memcpy(name, cursor, nameSizeInBytes);
            cursor += nameSizeInBytes;
            attribute.setName(name);

            size_t typeSizeInBytes = sizeof(MemberType);
            MemberType type;
            memcpy(&type, cursor, typeSizeInBytes);
            cursor += typeSizeInBytes;
            attribute.setType(type);

            rScript->addAttribute(attribute);
        }
        */
        delete[] fileBuffer;
    }
    
    return rScript;
}



