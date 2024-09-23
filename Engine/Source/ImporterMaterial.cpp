#include "SaveLoadMaterial.h"
#include "EngineApp.h"
#include "ModuleEngineResource.h"
#include "ModuleFileSystem.h"
#include "ImporterMaterial.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"
#include "Archive.h"


#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

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

    return EngineApp->GetEngineResource()->ImportFile(pngName.c_str(), uid++, modifyAssets);
}

ResourceMaterial* Importer::Material::GltfImport(const char* filePath, const tinygltf::Model& tinyModel, const tinygltf::Material& tinyMaterial, unsigned int& uid, std::unordered_map<unsigned int, unsigned int>& importedTextures, bool modifyAssets)
{
    const tinygltf::PbrMetallicRoughness& material = tinyMaterial.pbrMetallicRoughness;
    float baseColorFactor[4] = { static_cast<float>(material.baseColorFactor[0]), static_cast<float>(material.baseColorFactor[1]), static_cast<float>(material.baseColorFactor[2]), static_cast<float>(material.baseColorFactor[3]) };
    float metalicFactor = static_cast<float>(material.metallicFactor);
    float roughnessFactor = static_cast<float>(material.roughnessFactor);
    float emissiveFactor[3] = { static_cast<float>(tinyMaterial.emissiveFactor[0]), static_cast<float>(tinyMaterial.emissiveFactor[1]), static_cast<float>(tinyMaterial.emissiveFactor[2]) };
    unsigned int baseColorTex = 0;
    unsigned int metallicRoughTex = 0;
    unsigned int normalTex = 0;
    unsigned int emissiveTex = 0;

    int baseColorTexIdx = material.baseColorTexture.index;
    if (baseColorTexIdx >= 0)
    {
        if (importedTextures.find(baseColorTexIdx) == importedTextures.end())
        {
            const std::string textPath = tinyModel.images[tinyModel.textures[baseColorTexIdx].source].uri;
            baseColorTex = ImportTexture(filePath, textPath, uid, modifyAssets);
            importedTextures[baseColorTexIdx] = baseColorTex;
        }
        else
        {
            baseColorTex = importedTextures[baseColorTexIdx];
        }
    }
    int metalRoughTexIdx = material.metallicRoughnessTexture.index;
    if (metalRoughTexIdx >= 0)
    {
        if (importedTextures.find(metalRoughTexIdx) == importedTextures.end())
        {
            const std::string textPath = tinyModel.images[tinyModel.textures[metalRoughTexIdx].source].uri;
            metallicRoughTex = ImportTexture(filePath, textPath, uid, modifyAssets);
            importedTextures[metalRoughTexIdx] = metallicRoughTex;
        }
        else
        {
            metallicRoughTex = importedTextures[metalRoughTexIdx];
        }
    }


    const char* name = tinyMaterial.name.c_str();


    for (const auto& content : tinyMaterial.additionalValues)
    {
        if (content.first == "normalTexture")
        {
            const int indexValue = content.second.TextureIndex();
    
            if (indexValue) {
                int normalIndex = indexValue;
                if (importedTextures.find(normalIndex) == importedTextures.end())
                {
                    std::string textPath = tinyModel.images[tinyModel.textures[normalIndex].source].uri;
                    normalTex = ImportTexture(filePath, textPath, uid, modifyAssets);
                    importedTextures[normalIndex] = normalTex;
                }
                else
                {
                    normalTex = importedTextures[normalIndex];
                }
            }
        }
    }

    if (tinyMaterial.emissiveTexture.index != -1)
    {
        if (importedTextures.find(tinyMaterial.emissiveTexture.index) == importedTextures.end())
        {
            std::string textPath = tinyModel.images[tinyModel.textures[tinyMaterial.emissiveTexture.index].source].uri;
            emissiveTex = ImportTexture(filePath, textPath, uid, modifyAssets);
            importedTextures[tinyMaterial.emissiveTexture.index] = emissiveTex;
        }
        else
        {
            emissiveTex = importedTextures[tinyMaterial.emissiveTexture.index];
        }
    }
    ResourceMaterial* rMaterial = new ResourceMaterial(uid++, baseColorFactor, metalicFactor, roughnessFactor, emissiveFactor, baseColorTex, metallicRoughTex, normalTex, emissiveTex);
    Importer::Material::Save(rMaterial);
    return rMaterial;
}

ResourceMaterial* Importer::Material::MatImport(const char* filePath, unsigned int uid)
{
    char* fileBuffer = nullptr;
    ResourceMaterial* rMaterial = nullptr;
    if (App->GetFileSystem()->Load(filePath, &fileBuffer) > 0 && fileBuffer)
    {
        Archive doc(fileBuffer);
        delete[] fileBuffer;
        JsonObject root = doc.GetRootObject();
        std::string name = root.GetString("Name");
        float baseColorFactor[4];
        root.GetFloats("BaseColorFactor", baseColorFactor);
        float metalicFactor = root.GetFloat("MetallicFactor");
        float roughnessFactor = root.GetFloat("RoughnessFactor");
        float emissiveFactor[3];
        root.GetFloats("EmissiveFactor", emissiveFactor);
        unsigned int baseColorTex = root.GetInt("BaseColorTex");
        unsigned int metallicRoughTex = root.GetInt("MetallicRoughTex");
        unsigned int normalTex = root.GetInt("NormalTex");
        unsigned int emissiveTex = root.GetInt("EmissiveTex");
        bool enableBaseColorTex = root.GetBool("BaseColorEnabled");
        bool enableMetallicRoughnessTex = root.GetBool("MetallicRoughnessEnabled");
        bool enableNormalTex = root.GetBool("NormalMapEnabled");
        bool enableEmissiveTex = root.GetBool("EmissiveEnabled");

        rMaterial = new ResourceMaterial(uid, name.c_str(), baseColorFactor, metalicFactor, roughnessFactor, emissiveFactor, baseColorTex, metallicRoughTex, normalTex, emissiveTex, enableBaseColorTex, enableMetallicRoughnessTex, enableNormalTex, enableEmissiveTex);
        Importer::Material::Save(rMaterial);
    }
    return rMaterial;
}

ResourceMaterial* Importer::Material::ImportDefault()
{
    ResourceMaterial* rMaterial = nullptr;
    if (!App->GetFileSystem()->Exists("Library/99/999999999"))
    {
        float baseColorFactor[4] = { 1.f };
        float metalicFactor = 0.5f;
        float roughnessFactor = 0.5f;
        unsigned int baseColorTex = 0;
        unsigned int metallicRoughTex = 0;
        unsigned int normalTex = 0;
        unsigned int emissiveTex = 0;
        float emissiveFactor[4] = { 0.f };

        rMaterial = new ResourceMaterial(999999999, baseColorFactor, metalicFactor, roughnessFactor, emissiveFactor, baseColorTex, metallicRoughTex, normalTex, emissiveTex);
        Importer::Material::Save(rMaterial);
    }
    return rMaterial;
}