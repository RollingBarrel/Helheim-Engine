#include <string>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterScript.h"
#include "ResourceScript.h"


ResourceScript* Importer::Script::Import(const char* filePath, unsigned int uid)
{
    std::string filename = (filePath);
    std::vector<std::pair<std::string, std::string>> attributes = parseHeaderFile(filename);

    std::cout << "Attributes found in " << filename << ":" << std::endl;
    for (const auto& attribute : attributes) {
        std::cout << "Type: " << attribute.second << ", Name: " << attribute.first << std::endl;
    }

    ScriptAttribute scriptAttributes[10];
    

    ResourceScript* rScript = new ResourceScript(uid, scriptAttributes);
    Importer::Script::Save(rScript);
    return rScript;
}

void Importer::Script::Save(const ResourceScript* rTexture)
{
    // TODO: Save also UID (and assetsFile?)
    unsigned int header[7] = {
        rTexture->GetWidth(),
        rTexture->GetHeight(),
        rTexture->GetInternalFormat(),
        rTexture->GetTexFormat(),
        rTexture->GetDataType(),
        rTexture->GetMipLevels(),
        rTexture->GetNumPixels()
    };

    unsigned int numPixels = rTexture->GetNumPixels();
    bool hasAlpha = rTexture->HasAlpha();
    unsigned int size = sizeof(header) +
        sizeof(hasAlpha) +
        sizeof(unsigned char) * numPixels;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    bytes = sizeof(hasAlpha);
    memcpy(cursor, &hasAlpha, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * numPixels;
    memcpy(cursor, rTexture->GetPixels(), bytes);
    cursor += bytes;

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rTexture->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceScript* Importer::Script::Load(const char* filePath, unsigned int uid)
{
    char* fileBuffer;
    ResourceTexture* texture = nullptr;

    if (App->GetFileSystem()->Load(filePath, &fileBuffer))
    {
        char* cursor = fileBuffer;
        unsigned int header[7];
        unsigned int bytes = sizeof(header);
        memcpy(header, cursor, bytes);
        cursor += bytes;
        unsigned int width = header[0];
        unsigned int height = header[1];
        unsigned int internalFormat = header[2];
        unsigned int texFormat = header[3];
        unsigned int dataType = header[4];
        unsigned int mipLevels = header[5];
        unsigned int numPixels = header[6];


        bool hasAlpha;
        bytes = sizeof(hasAlpha);
        memcpy(&hasAlpha, cursor, bytes);
        cursor += bytes;

        bytes = sizeof(unsigned char) * numPixels;
        unsigned char* pixels = nullptr;
        pixels = new unsigned char[numPixels];
        memcpy(pixels, cursor, bytes);

        texture = new ResourceTexture(uid, width, height, internalFormat, texFormat, dataType, mipLevels, numPixels, pixels, hasAlpha);
        unsigned int texId = texture->CreateTexture();

        delete[] fileBuffer;
    }

    return texture;
}

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Function to parse a header file and extract attribute names and types
std::vector<std::pair<std::string, std::string>> parseHeaderFile(const std::string& filename) {
    std::vector<std::pair<std::string, std::string>> attributes;

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file " << filename << std::endl;
        return attributes;
    }
    std::string serializingMacro = "SERIALIZE_FIELD";

    std::string line;
    while (std::getline(file, line)) {
        std::smatch match;
        if (std::regex_search(line, match, attributeRegex)) {
            if (match.size() == 2) {
                std::string attributeName = match[1].str();
                std::string attributeType = line.substr(0, match.position(1));
                attributes.push_back(std::make_pair(attributeName, attributeType));
            }
        }
    }

    return attributes;
}
