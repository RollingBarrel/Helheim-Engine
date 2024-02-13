#include "Application.h"
#include "ModuleFileSystem.h"
#include "ImporterMaterial.h"
#include "ImporterTexture.h"

#include "ResourceMaterial.h"
#include "ResourceTexture.h"

#include "Algorithm/Random/LCG.h"

ResourceMaterial* Importer::Material::Import(const tinygltf::Model& model, const tinygltf::Material& material)
{
    ResourceMaterial* rMaterial = new ResourceMaterial(math::LCG().Int());

    auto it = material.extensions.find("KHR_materials_pbrSpecularGlossiness");
    if (it != material.extensions.end()) {
        const tinygltf::Value& extensionMap = it->second;

        if (extensionMap.Has("diffuseFactor")) {
            const tinygltf::Value& diffuseFactor = extensionMap.Get("diffuseFactor");
            if (diffuseFactor.IsArray()) {
                for (int i = 0; i < 4; ++i) {
                    rMaterial->mDiffuseFactor[i] = static_cast<float>(diffuseFactor.Get(i).Get<double>());
                }
            }
        }

        if (extensionMap.Has("specularFactor")) {
            const tinygltf::Value& specularFactor = extensionMap.Get("specularFactor");
            if (specularFactor.IsArray()) {
                for (int i = 0; i < 3; ++i) {
                    rMaterial->mSpecularFactor[i] = static_cast<float>(specularFactor.Get(i).Get<double>());
                }
            }
        }

        if (extensionMap.Has("glossinessFactor")) {
            const tinygltf::Value& glossinessFactor = extensionMap.Get("glossinessFactor");
            if (glossinessFactor.IsNumber()) {
                rMaterial->mGlossinessFactor = static_cast<float>(glossinessFactor.Get<double>());
            }
        }

        if (extensionMap.Has("diffuseTexture")) {
            const tinygltf::Value& diffuseTexture = extensionMap.Get("diffuseTexture");

            if (diffuseTexture.IsObject()) {
                const tinygltf::Value& indexValue = diffuseTexture.Get("index");

                if (indexValue.IsInt()) {
                    int diffuseTextureIndex = indexValue.Get<int>();
                    const tinygltf::Texture& diffuseMap = model.textures[diffuseTextureIndex];
                    const tinygltf::Image& image = model.images[diffuseMap.source];
                    const char* imageUri = image.uri.c_str();

                    ResourceTexture* diffuseTexture = new ResourceTexture(math::LCG().Int());
                    Importer::Texture::Import(imageUri, diffuseTexture);
                    rMaterial->mDiffuseTexture = diffuseTexture;

                    rMaterial->mEnableDiffuseTexture = true;
                }
            }
        }

        if (extensionMap.Has("specularGlossinessTexture")) {
            const tinygltf::Value& specularGlossinessTexture = extensionMap.Get("specularGlossinessTexture");

            if (specularGlossinessTexture.IsObject()) {
                const tinygltf::Value& indexValue = specularGlossinessTexture.Get("index");

                if (indexValue.IsInt()) {
                    int specularGlossinessIndex = indexValue.Get<int>();
                    const tinygltf::Texture& specularMap = model.textures[specularGlossinessIndex];
                    const tinygltf::Image& image = model.images[specularMap.source];
                    const char* imageUri = image.uri.c_str();

                    ResourceTexture* specularTexture = new ResourceTexture(math::LCG().Int());
                    Importer::Texture::Import(imageUri, specularTexture);
                    rMaterial->mSpecularGlossinessTexture = specularTexture;

                    rMaterial->mEnableSpecularGlossinessTexture = true;

                    if (specularTexture->mHasAlpha) {
                        rMaterial->mEnableShinessMap = true;
                    }
                }
            }
        }

        if (material.additionalValues.size() > 0)
        {
            for (const auto& content : material.additionalValues)
            {
                if (content.first == "normalTexture")
                {
                    const int indexValue = content.second.TextureIndex();

                    if (indexValue) {
                        int normalIndex = indexValue;
                        const tinygltf::Texture& normalMap = model.textures[normalIndex];
                        const tinygltf::Image& image = model.images[normalMap.source];
                        const char* imageUri = image.uri.c_str();

                        ResourceTexture* normalTexture = new ResourceTexture(math::LCG().Int());
                        Importer::Texture::Import(imageUri, normalTexture);
                        rMaterial->mNormalTexture = normalTexture;

                        rMaterial->mEnableNormalMap = true;
                    }
                }
            }
        }

    }
    else {
        // Generic case for NON-KHR_materials_pbrSpecularGlossiness (e.g, first assignment model)
        if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
        {
            const tinygltf::Texture& texture = model.textures[material.pbrMetallicRoughness.baseColorTexture.index];
            const tinygltf::Image& image = model.images[texture.source];
            const char* imageUri = image.uri.c_str();

            ResourceTexture* diffuseTexture = new ResourceTexture(math::LCG().Int());
            Importer::Texture::Import(imageUri, diffuseTexture);
            rMaterial->mDiffuseTexture = diffuseTexture;

            rMaterial->mEnableDiffuseTexture = true;
        }

    }

    return rMaterial;
}

void Importer::Material::Save(const ResourceMaterial* ourMaterial)
{
    unsigned int texturesUID[3] = { (ourMaterial->mDiffuseTexture != nullptr) ? ourMaterial->mDiffuseTexture->GetUID() : 0,
                                    (ourMaterial->mSpecularGlossinessTexture != nullptr) ? ourMaterial->mSpecularGlossinessTexture->GetUID() : 0,
                                    (ourMaterial->mNormalTexture != nullptr) ? ourMaterial->mNormalTexture->GetUID() : 0 };

    bool enables[4] = { ourMaterial->mDiffuseTexture, ourMaterial->mEnableSpecularGlossinessTexture, ourMaterial->mEnableNormalMap, ourMaterial->mEnableShinessMap }; 

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
    memcpy(cursor, &ourMaterial->mDiffuseFactor, bytes);
    cursor += bytes;

    bytes = sizeof(float) * 3;
    memcpy(cursor, &ourMaterial->mSpecularFactor, bytes);
    cursor += bytes;

    bytes = sizeof(float);
    memcpy(cursor, &ourMaterial->mGlossinessFactor, bytes);
    cursor += bytes;

    //TODO Change name for random UID
    std::string path = LIBRARY_MATERIAL_PATH;
    path += std::to_string(ourMaterial->GetUID());
    path += ".mat";

    App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;

}

void Importer::Material::Load(ResourceMaterial* ourMaterial, const char* fileName)
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
    ourMaterial->mDiffuseTexture = new ResourceTexture();
    if(texturesUID[0])
        Importer::Texture::Load(ourMaterial->mDiffuseTexture, std::to_string(texturesUID[0]).c_str());
    ourMaterial->mSpecularGlossinessTexture = new ResourceTexture();
    if (texturesUID[1])
        Importer::Texture::Load(ourMaterial->mSpecularGlossinessTexture, std::to_string(texturesUID[1]).c_str());
    ourMaterial->mNormalTexture = new ResourceTexture();
    if (texturesUID[2])
        Importer::Texture::Load(ourMaterial->mNormalTexture, std::to_string(texturesUID[2]).c_str());

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


