#include "Application.h"
#include "ModuleFileSystem.h"
#include "ImporterMaterial.h"
#include "ImporterTexture.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "Algorithm/Random/LCG.h"

ResourceMaterial* Importer::Material::Import(const unsigned int uid, const tinygltf::Model& tinyModel, const tinygltf::Material& tinyMaterial)
{
    float4 diffuseFactor = float4(); 
    float3 specularFactor = float3();
    float GlossinessFactor = 0.0f;
    ResourceTexture* diffuseTexture = nullptr; 
    ResourceTexture* specularGlossinessTexture = nullptr;
    ResourceTexture* normalTexture = nullptr;

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

        if (extensionMap.Has("diffuseTexture")) {
            const tinygltf::Value& diffuseTextureValue = extensionMap.Get("diffuseTexture");

            if (diffuseTextureValue.IsObject()) {
                const tinygltf::Value& indexValue = diffuseTextureValue.Get("index");

                if (indexValue.IsInt()) {
                    int diffuseTextureIndex = indexValue.Get<int>();
                    const tinygltf::Texture& diffuseMap = tinyModel.textures[diffuseTextureIndex];
                    const tinygltf::Image& image = tinyModel.images[diffuseMap.source];
                    const char* imageUri = image.uri.c_str();

                    unsigned int uidTexture = math::LCG().Int();
                    diffuseTexture = Importer::Texture::Import(imageUri, uidTexture);
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
                    const char* imageUri = image.uri.c_str();

                    unsigned int uidTexture = math::LCG().Int();
                    specularGlossinessTexture = Importer::Texture::Import(imageUri, uidTexture);
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
                        const char* imageUri = image.uri.c_str();

                        unsigned int uidTexture = math::LCG().Int();
                        normalTexture = Importer::Texture::Import(imageUri, uidTexture);
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
            const char* imageUri = image.uri.c_str();

            unsigned int uidTexture = math::LCG().Int();
            diffuseTexture = Importer::Texture::Import(imageUri, uidTexture);
        }

    }
    ResourceMaterial* material = new ResourceMaterial(uid, diffuseFactor, specularFactor, GlossinessFactor, diffuseTexture, specularGlossinessTexture, normalTexture);
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
        ourMaterial->IsShinessMapEnabled()};

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

    bytes = sizeof(float) * 4;
    memcpy(cursor, &ourMaterial->GetDiffuseFactor(), bytes);
    cursor += bytes;

    bytes = sizeof(float) * 3;
    memcpy(cursor, &ourMaterial->GetSpecularFactor(), bytes);
    cursor += bytes;

    bytes = sizeof(float);
    memcpy(cursor, &ourMaterial->GetGlossinessFactor(), bytes);
    cursor += bytes;

    //TODO Change name for random UID
    std::string path = LIBRARY_MATERIAL_PATH;
    path += std::to_string(ourMaterial->GetUID());
    path += ".mat";

    App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;

}

ResourceMaterial* Importer::Material::Load(const unsigned int uid, const char* fileName)
{
    char* fileBuffer;

    std::string path = LIBRARY_MATERIAL_PATH;
    path += fileName;
    path += ".mat";

    App->GetFileSystem()->Load(path.c_str(), &fileBuffer);

    char* cursor = fileBuffer;
    unsigned int texturesUID[3];
    unsigned int bytes = sizeof(texturesUID);

    memcpy(texturesUID, cursor, bytes);
    cursor += bytes;
    //ourMaterial->mDiffuseTexture = new ResourceTexture();
    //if(texturesUID[0])
    //    Importer::Texture::Load(ourMaterial->mDiffuseTexture, std::to_string(texturesUID[0]).c_str());
    //ourMaterial->mSpecularGlossinessTexture = new ResourceTexture();
    //if (texturesUID[1])
    //    Importer::Texture::Load(ourMaterial->mSpecularGlossinessTexture, std::to_string(texturesUID[1]).c_str());
    //ourMaterial->mNormalTexture = new ResourceTexture();
    //if (texturesUID[2])
    //    Importer::Texture::Load(ourMaterial->mNormalTexture, std::to_string(texturesUID[2]).c_str());

    bool enables[4];
    bytes = sizeof(enables);
    memcpy(enables, cursor, bytes);
    cursor += bytes;
    ourMaterial->mEnableDiffuseTexture = enables[0];
    ourMaterial->mEnableSpecularGlossinessTexture = enables[1];
    ourMaterial->mEnableNormalMap = enables[2];
    ourMaterial->mEnableShinessMap = enables[3];

    bytes = sizeof(float) * 4;
    memcpy(&ourMaterial->mDiffuseFactor, cursor, bytes);
    cursor += bytes;

    bytes = sizeof(float) * 3;
    memcpy(&ourMaterial->mSpecularFactor, cursor, bytes);
    cursor += bytes;

    bytes = sizeof(float);
    memcpy(&ourMaterial->mGlossinessFactor, cursor, bytes);
    cursor += bytes;
}


