#include "Application.h"
#include "ModuleResource.h"
#include "ModuleFileSystem.h"
#include "ImporterMaterial.h"
#include "ImporterTexture.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"


static unsigned int ImportTexture(const char* filePath, const std::string& texPath, unsigned int& uid, bool modifyAssets)
{
    std::string pngName = filePath;
    unsigned int filePos = pngName.find_last_of('/');
    pngName = pngName.substr(0, filePos + 1);
    pngName += texPath;
    if (!modifyAssets)
    {
        size_t sizeUntilName = pngName.find_last_of('/') + 1;
        std::string extension = pngName.substr(pngName.find_last_of('.'));
        std::string name = pngName.substr(sizeUntilName, pngName.length() - sizeUntilName - (pngName.length() - pngName.find_last_of('.')));
        pngName = ASSETS_TEXTURE_PATH;
        pngName += name;
        pngName += extension;
    }

    return App->GetResource()->ImportFile(pngName.c_str(), uid++, modifyAssets);
}

ResourceMaterial* Importer::Material::Import(const char* filePath, const tinygltf::Model& tinyModel, const tinygltf::Material& tinyMaterial, unsigned int& uid, bool modifyAssets)
{
    const tinygltf::PbrMetallicRoughness& material = tinyMaterial.pbrMetallicRoughness;
    float baseColorFactor[4] = { static_cast<float>(material.baseColorFactor[0]), static_cast<float>(material.baseColorFactor[1]), static_cast<float>(material.baseColorFactor[2]), static_cast<float>(material.baseColorFactor[3]) };
    float metalicFactor = static_cast<float>(material.metallicFactor);
    float roughnessFactor = static_cast<float>(material.roughnessFactor);
    unsigned int baseColorTex = 0;
    unsigned int metallicRoughTex = 0;
    unsigned int normalTex = 0;

    int baseColorTexIdx = material.baseColorTexture.index;
    if (baseColorTexIdx >= 0)
    {
        const std::string textPath = tinyModel.images[tinyModel.textures[baseColorTexIdx].source].uri;
        baseColorTex = ImportTexture(filePath, textPath, uid, modifyAssets);
    }
    int metalRoughTexIdx = material.metallicRoughnessTexture.index;
    if (metalRoughTexIdx >= 0)
    {
        const std::string textPath = tinyModel.images[tinyModel.textures[metalRoughTexIdx].source].uri;
        metallicRoughTex = ImportTexture(filePath, textPath, uid, modifyAssets);;
    }


    const char* name = tinyMaterial.name.c_str();

    //if (tinyMaterial.pbrMetallicRoughness.baseColorTexture.index > -1)
    //{
    //    //TODO Import PBR Texture
    //    const tinygltf::Texture& texture = tinyModel.textures[tinyMaterial.pbrMetallicRoughness.baseColorTexture.index];
    //    const tinygltf::Image& image = tinyModel.images[texture.source];
    //
    //    for (int i = 0; i < 4; ++i) {
    //        diffuseFactor[i] = static_cast<float>(tinyMaterial.pbrMetallicRoughness.baseColorFactor[i]);
    //    }
    //   
    //    std::string pngName = filePath;
    //    unsigned filePos = pngName.find_last_of('/');
    //    pngName = pngName.substr(0, filePos + 1);
    //    pngName.append(tinyModel.images[texture.source].uri);
    //    if (!modifyAssets)
    //    {
    //        size_t sizeUntilName = pngName.find_last_of('/') + 1;
    //        std::string extension = pngName.substr(pngName.find_last_of('.'));
    //        std::string name = pngName.substr(sizeUntilName, pngName.length() - sizeUntilName - (pngName.length() - pngName.find_last_of('.')));
    //        pngName = ASSETS_TEXTURE_PATH;
    //        pngName += name;
    //        pngName += extension;
    //    }
    //
    //    diffuseTexture = App->GetResource()->ImportFile(pngName.c_str(), uid++, modifyAssets);
    //
    //    if (tinyMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index > -1)
    //    {
    //        const tinygltf::Texture& texture = tinyModel.textures[tinyMaterial.pbrMetallicRoughness.metallicRoughnessTexture.index];
    //        const tinygltf::Image& image = tinyModel.images[texture.source];
    //
    //        std::string pngName = filePath;
    //        unsigned filePos = pngName.find_last_of('/');
    //        pngName = pngName.substr(0, filePos + 1);
    //        pngName.append(tinyModel.images[texture.source].uri);
    //        if (!modifyAssets)
    //        {
    //            size_t sizeUntilName = pngName.find_last_of('/') + 1;
    //            std::string extension = pngName.substr(pngName.find_last_of('.'));
    //            std::string name = pngName.substr(sizeUntilName, pngName.length() - sizeUntilName - (pngName.length() - pngName.find_last_of('.')));
    //            pngName = ASSETS_TEXTURE_PATH;
    //            pngName += name;
    //            pngName += extension;
    //        }
    //
    //        //metallicRoughness = App->GetResource()->ImportFile(pngName.c_str(), uid++, modifyAssets);
    //    }
    //}

    if (tinyMaterial.additionalValues.size() > 0)
    {
        for (const auto& content : tinyMaterial.additionalValues)
        {
            if (content.first == "normalTexture")
            {
                const int indexValue = content.second.TextureIndex();

                if (indexValue) {
                    int normalIndex = indexValue;

                    std::string textPath = tinyModel.images[tinyModel.textures[normalIndex].source].uri;
                    normalTex = ImportTexture(filePath, textPath, uid, modifyAssets);
                }
            }
        }
    }

    ResourceMaterial* rMaterial = new ResourceMaterial(uid++, baseColorFactor, metalicFactor, roughnessFactor, baseColorTex, metallicRoughTex, normalTex);
    Importer::Material::Save(rMaterial);
    return rMaterial;
}

void Importer::Material::Save(const ResourceMaterial* rMaterial)
{
    unsigned int texturesUID[3] = { (rMaterial->GetBaseColorTexture() != nullptr) ? rMaterial->GetBaseColorTexture()->GetUID() : 0,
                                    (rMaterial->GetMetallicRoughnessTexture() != nullptr) ? rMaterial->GetMetallicRoughnessTexture()->GetUID() : 0,
                                    (rMaterial->GetNormalTexture() != nullptr) ? rMaterial->GetNormalTexture()->GetUID() : 0};

    bool enables[3] = { 
        rMaterial->IsBaseColorEnabled(),
        rMaterial->IsMetallicRoughnessEnabled(),
        rMaterial->IsNormalMapEnabled()};

    unsigned int size = sizeof(texturesUID) +
                        sizeof(enables) +
                        sizeof(float) * 4 + 
                        sizeof(float) + 
                        sizeof(float);

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(texturesUID);
    memcpy(cursor, texturesUID, bytes);
    cursor += bytes;

    bytes = sizeof(enables);
    memcpy(cursor, enables, bytes);
    cursor += bytes;

    float4 baseColorFactor = rMaterial->GetBaseColorFactor();
    bytes = sizeof(float) * 4;
    memcpy(cursor, &baseColorFactor, bytes);
    cursor += bytes;

    float metallicFactorFactor = rMaterial->GetMetallicFactor();
    bytes = sizeof(float);
    memcpy(cursor, &metallicFactorFactor, bytes);
    cursor += bytes;

    float roughnessFactor = rMaterial->GetRoughnessFactor();
    bytes = sizeof(float);
    memcpy(cursor, &roughnessFactor, bytes);
    cursor += bytes;

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rMaterial->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceMaterial* Importer::Material::Load(const char* fileName, const unsigned int uid )
{
    char* fileBuffer = nullptr;
    ResourceMaterial* ret = nullptr;
    if (App->GetFileSystem()->Load(fileName, &fileBuffer))
    {

        char* cursor = fileBuffer;
        unsigned int texturesUID[3];
        unsigned int bytes = sizeof(texturesUID);

        memcpy(texturesUID, cursor, bytes);
        cursor += bytes;

        bool enables[3];
        bytes = sizeof(enables);
        memcpy(enables, cursor, bytes);
        cursor += bytes;

        bool enablebaseColorTexture = enables[0];
        bool enableMetallicRoughnessTexture = enables[1];
        bool enableNormalTexture = enables[2];

        float4 baseColorFactor;
        bytes = sizeof(float) * 4;
        memcpy(&baseColorFactor, cursor, bytes);
        cursor += bytes;

        float metallicFactor;
        bytes = sizeof(float);
        memcpy(&metallicFactor, cursor, bytes);
        cursor += bytes;

        float roughnessFator;
        bytes = sizeof(float);
        memcpy(&roughnessFator, cursor, bytes);
        cursor += bytes;
        ret = new ResourceMaterial(uid, baseColorFactor, metallicFactor, roughnessFator, texturesUID[0], texturesUID[1], texturesUID[2]);
        Importer::Material::Save(ret);

        delete[] fileBuffer;
    }

    return ret;
}


