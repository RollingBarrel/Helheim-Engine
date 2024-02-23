#include "Application.h"
#include "ModuleResource.h"
#include "ModuleFileSystem.h"
#include "ImporterMaterial.h"
#include "ImporterTexture.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "Algorithm/Random/LCG.h"

ResourceMaterial* Importer::Material::Import(const char* filePath, const tinygltf::Model& tinyModel, const tinygltf::Material& tinyMaterial, const unsigned int uid)
{
    float4 diffuseFactor = float4::zero; 
    float3 specularFactor = float3::zero;
    float GlossinessFactor = 0.0f;
    unsigned int diffuseTexture = 0; 
    unsigned int specularGlossinessTexture = 0;
    unsigned int normalTexture = 0;

    auto it = tinyMaterial.extensions.find("KHR_materials_pbrSpecularGlossiness");
    if (it != tinyMaterial.extensions.end()) {
        const tinygltf::Value& extensionMap = it->second;

        if (extensionMap.Has("diffuseFactor")) {
            const tinygltf::Value& diffuseFactorValue = extensionMap.Get("diffuseFactor");
            if (diffuseFactorValue.IsArray()) {
                for (int i = 0; i < 4; ++i) {
                    diffuseFactor[i] = static_cast<float>(diffuseFactorValue.Get(i).Get<double>());
                }
            }
        }

        if (extensionMap.Has("specularFactor")) {
            const tinygltf::Value& specularFactorValue = extensionMap.Get("specularFactor");
            if (specularFactorValue.IsArray()) {
                for (int i = 0; i < 3; ++i) {
                    specularFactor[i] = static_cast<float>(specularFactorValue.Get(i).Get<double>());
                }
            }
        }

        if (extensionMap.Has("glossinessFactor")) {
            const tinygltf::Value& glossinessFactorValue = extensionMap.Get("glossinessFactor");
            if (glossinessFactorValue.IsNumber()) {
                GlossinessFactor = static_cast<float>(glossinessFactorValue.Get<double>());
            }
        }

        //TODO IMPORT DIFFUSE COLOR ALWAYS 
        if (extensionMap.Has("diffuseTexture")) {
            const tinygltf::Value& diffuseTextureValue = extensionMap.Get("diffuseTexture");

            if (diffuseTextureValue.IsObject()) {
                const tinygltf::Value& indexValue = diffuseTextureValue.Get("index");

                if (indexValue.IsInt()) {
                    int diffuseTextureIndex = indexValue.Get<int>();
                    const tinygltf::Texture& diffuseMap = tinyModel.textures[diffuseTextureIndex];
                    const tinygltf::Image& image = tinyModel.images[diffuseMap.source];

                    std::string pngName = filePath;
                    unsigned filePos = pngName.find_last_of('/');
                    pngName = pngName.substr(0, filePos + 1);
                    pngName.append(tinyModel.images[diffuseTextureIndex].uri);

                    diffuseTexture = App->GetResource()->ImportFile(pngName.c_str());
                }
            }
        }

        if (extensionMap.Has("specularGlossinessTexture")) {
            const tinygltf::Value& specularGlossinessTextureValue = extensionMap.Get("specularGlossinessTexture");

            if (specularGlossinessTextureValue.IsObject()) {
                const tinygltf::Value& indexValue = specularGlossinessTextureValue.Get("index");

                if (indexValue.IsInt()) {
                    int specularGlossinessIndex = indexValue.Get<int>();
                    const tinygltf::Texture& specularMap = tinyModel.textures[specularGlossinessIndex];
                    const tinygltf::Image& image = tinyModel.images[specularMap.source];

                    std::string pngName = filePath;
                    unsigned filePos = pngName.find_last_of('/');
                    pngName = pngName.substr(0, filePos + 1);
                    pngName.append(tinyModel.images[specularGlossinessIndex].uri);

                    specularGlossinessTexture = App->GetResource()->ImportFile(pngName.c_str());
                }
            }
        }

        if (tinyMaterial.additionalValues.size() > 0)
        {
            for (const auto& content : tinyMaterial.additionalValues)
            {
                if (content.first == "normalTexture")
                {
                    const int indexValue = content.second.TextureIndex();

                    if (indexValue) {
                        int normalIndex = indexValue;
                        const tinygltf::Texture& normalMap = tinyModel.textures[normalIndex];
                        const tinygltf::Image& image = tinyModel.images[normalMap.source];

                        std::string pngName = filePath;
                        unsigned filePos = pngName.find_last_of('/');
                        pngName = pngName.substr(0, filePos + 1);
                        pngName.append(tinyModel.images[normalIndex].uri);

                        normalTexture = App->GetResource()->ImportFile(pngName.c_str());
                    }
                }
            }
        }

    }
    else {
        // Generic case for NON-KHR_materials_pbrSpecularGlossiness (e.g, first assignment model)
        if (tinyMaterial.pbrMetallicRoughness.baseColorTexture.index >= 0)
        {
            const tinygltf::Texture& texture = tinyModel.textures[tinyMaterial.pbrMetallicRoughness.baseColorTexture.index];
            const tinygltf::Image& image = tinyModel.images[texture.source];

            std::string pngName = filePath;
            unsigned filePos = pngName.find_last_of('/');
            pngName = pngName.substr(0, filePos + 1);
            pngName.append(tinyModel.images[tinyMaterial.pbrMetallicRoughness.baseColorTexture.index].uri);

            diffuseTexture = App->GetResource()->ImportFile(pngName.c_str());
        }

    }
    ResourceMaterial* material = new ResourceMaterial(uid, filePath, diffuseFactor, specularFactor, GlossinessFactor, diffuseTexture, specularGlossinessTexture, normalTexture);
    return material;
}

void Importer::Material::Save(const ResourceMaterial* ourMaterial)
{
    unsigned int texturesUID[3] = { (ourMaterial->GetDiffuseTexture() != nullptr) ? ourMaterial->GetDiffuseTexture()->GetUID() : 0,
                                    (ourMaterial->GetSpecularGlossinessTexture() != nullptr) ? ourMaterial->GetSpecularGlossinessTexture()->GetUID() : 0,
                                    (ourMaterial->GetNormalTexture() != nullptr) ? ourMaterial->GetNormalTexture()->GetUID() : 0};

    bool enables[4] = { 
        ourMaterial->IsDiffuseTextureEnabled(),
        ourMaterial->IsSpecularGlossinessTextureEnabled(),
        ourMaterial->IsNormalMapEnabled(),
        ourMaterial->IsShininessMapEnabled()};

    unsigned int size = sizeof(texturesUID) +
                        sizeof(enables) +
                        sizeof(float) * 4 + 
                        sizeof(float) * 3 + 
                        sizeof(float);

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(texturesUID);
    memcpy(cursor, texturesUID, bytes);
    cursor += bytes;

    bytes = sizeof(enables);
    memcpy(cursor, enables, bytes);
    cursor += bytes;

    float4 diffuseFactor = ourMaterial->GetDiffuseFactor();
    bytes = sizeof(float) * 4;
    memcpy(cursor, &diffuseFactor, bytes);
    cursor += bytes;

    float3 specularFactor = ourMaterial->GetSpecularFactor();
    bytes = sizeof(float) * 3;
    memcpy(cursor, &specularFactor, bytes);
    cursor += bytes;

    float glossinessFactor = ourMaterial->GetGlossinessFactor();
    bytes = sizeof(float);
    memcpy(cursor, &glossinessFactor, bytes);
    cursor += bytes;

    App->GetFileSystem()->Save(ourMaterial->GetLibraryFile(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;

}

ResourceMaterial* Importer::Material::Load(const char* fileName, const unsigned int uid )
{
    char* fileBuffer;
    ResourceMaterial* ret = nullptr;
    if (App->GetFileSystem()->Load(fileName, &fileBuffer))
    {

        char* cursor = fileBuffer;
        unsigned int texturesUID[3];
        unsigned int bytes = sizeof(texturesUID);

        memcpy(texturesUID, cursor, bytes);
        cursor += bytes;

        bool enables[4];
        bytes = sizeof(enables);
        memcpy(enables, cursor, bytes);
        cursor += bytes;

        bool enableDiffuseTexture = enables[0];
        bool enableSpecularGlossinessTexture = enables[1];
        bool enableNormalTexture = enables[2];
        bool enableShininessTexture = enables[3];

        float4 diffuseFactor;
        bytes = sizeof(float) * 4;
        memcpy(&diffuseFactor, cursor, bytes);
        cursor += bytes;

        float3 specularFactor;
        bytes = sizeof(float) * 3;
        memcpy(&specularFactor, cursor, bytes);
        cursor += bytes;

        float glossinessFator;
        bytes = sizeof(float);
        memcpy(&glossinessFator, cursor, bytes);
        cursor += bytes;
        ret = new ResourceMaterial(uid, fileName, diffuseFactor, specularFactor, glossinessFator, texturesUID[0], texturesUID[1], texturesUID[2]);
    }

    return ret;
}


