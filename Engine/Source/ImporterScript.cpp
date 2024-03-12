#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterScript.h"
#include "ResourceScript.h"


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
    size_t sizeAttributes = rScript->GetAttributes().size();
    size_t sizeDisplayAttributes = rScript->GetDisplayAttributes().size();
    size_t sizeScriptName = rScript->GetScriptName().length();

    unsigned int size = sizeof(size_t) * 3 +
        sizeof(rScript->GetScriptName()) +
        sizeof(ScriptAttribute) * sizeAttributes +
        sizeof(ScriptAttribute) * sizeDisplayAttributes;
    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(size_t);
    memcpy(cursor, &sizeAttributes, bytes);
    cursor += bytes;
    unsigned int bytes = sizeof(size_t);
    memcpy(cursor, &sizeDisplayAttributes, bytes);
    cursor += bytes;
    unsigned int bytes = sizeof(size_t);
    memcpy(cursor, &sizeScriptName, bytes);
    cursor += bytes;

    //TODO: Redo the save of name
    unsigned int bytes = sizeof(rScript->GetScriptName());
    memcpy(cursor, &rScript->GetScriptName(), bytes);
    cursor += bytes;

    // TODO: Redo the save of Script Attributes 
    for (size_t i = 0; i < sizeAttributes; ++i)
    {
        const ScriptAttribute attribute = rScript->GetAttributes()[i];
        unsigned int bytes = sizeof(ScriptAttribute);
        memcpy(cursor, &attribute, bytes);
        cursor += bytes;
    }

    for (size_t i = 0; i < sizeDisplayAttributes; ++i)
    {
        const ScriptAttribute attribute = rScript->GetDisplayAttributes()[i];
        unsigned int bytes = sizeof(ScriptAttribute);
        memcpy(cursor, &attribute, bytes);
        cursor += bytes;
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
        char* cursor = fileBuffer;

        size_t sizeAttributes;
        unsigned int bytes = sizeof(size_t);
        memcpy(&sizeAttributes, cursor, bytes);
        cursor += bytes;
        size_t sizeDisplayAttributes;
        unsigned int bytes = sizeof(size_t);
        memcpy(&sizeDisplayAttributes, cursor, bytes);
        cursor += bytes;
        size_t sizeScriptName;
        unsigned int bytes = sizeof(size_t);
        memcpy(&sizeScriptName, cursor, bytes);
        cursor += bytes;

        std::string scriptName;
        memcpy(&scriptName, cursor, sizeScriptName);
        cursor += bytes;
        rScript->SetScriptName(scriptName);

        for (size_t i = 0; i < sizeAttributes; ++i)
        {
            ScriptAttribute attribute;
            unsigned int bytes = sizeof(ScriptAttribute);
            memcpy(&attribute, cursor, bytes);
            cursor += bytes;
            rScript->addAttribute(attribute);
        }

        for (size_t i = 0; i < sizeDisplayAttributes; ++i)
        {
            ScriptAttribute attribute;
            unsigned int bytes = sizeof(ScriptAttribute);
            memcpy(&attribute, cursor, bytes);
            cursor += bytes;
            rScript->addDisplayAttribute(attribute);

        }

        delete[] fileBuffer;
    }

    return rScript;
}

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
    bool display = false;
    bool serializable = true;

    // Read line by line in the .h file
    std::string line;
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
        if (firstWord == "DISPLAY") 
        {
            std::cout << "Found line starting with DISPLAY: " << trimmedLine << std::endl; // Errasable
            display = true;
        }
        else if (firstWord == "UNSERIALIZED") 
        {
            std::cout << "Found line starting with UNSERIALIZED: " << trimmedLine << std::endl; // Errasable
            serializable = false;
        }
        //else if (firstWord == "TOOLTIP") 
        //{
        //    std::cout << "Found line starting with TOOLTIP: " << trimmedLine << std::endl; // Errasable
        //    std::string parameter = trim(trimmedLine.substr(firstWord.length()), "()\" \t");
        //    std::cout << "Tooltip: " << parameter << std::endl;


        //    attribute.setTooltip(parameter);
        //}
        else if (firstWord == "RANGE") 
        {
            std::cout << "Found line starting with RANGE: " << trimmedLine << std::endl; // Errasable
            std::string parameters = trimmedLine.substr(firstWord.length());
            parameters = trim(parameters, "() \t");

            std::pair<int, int> range;

            size_t foundPos = parameters.find(',');
            if (foundPos != std::string::npos) 
            {
                range.first = std::stoi(trim(parameters.substr(0, foundPos), " \t"));
                range.second = std::stoi(trim(parameters.substr(foundPos + 1), " \t"));
            }
        }
        //else if (firstWord == "HEADER") 
        //{
        //    std::cout << "Found line starting with HEADER: " << trimmedLine << std::endl; // Errasable
        //    std::string parameter = trim(trimmedLine.substr(firstWord.length()), "() \t");

        //    attribute.setHeader(parameter);
        //}
        //else if (firstWord == "SPACE") 
        //{
        //    std::cout << "Found line starting with SPACE: " << trimmedLine << std::endl; // Errasable
        //    attribute.setHeader("");
        //}
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
            attribute.setName(name);

            if (display)
            {
                rScript->addDisplayAttribute(attribute);
            }
            if (serializable)
            {
                rScript->addAttribute(attribute);
            }
            attribute = ScriptAttribute();
            bool display = false;
            bool serializable = true;
        }
    }

    return true;
}
