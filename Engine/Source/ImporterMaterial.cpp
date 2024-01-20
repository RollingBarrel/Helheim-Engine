#include "ImporterMaterial.h"
#include "ImporterTexture.h"

void Importer::Material::Import(const tinygltf::Model& model, const tinygltf::Material& material, ResourceMaterial* rMaterial)
{
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

                    ResourceTexture* diffuseTexture = new ResourceTexture();
                    diffuseTexture->mTextureName = imageUri;
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

                    ResourceTexture* specularTexture = new ResourceTexture();
                    specularTexture->mTextureName = imageUri;
                    Importer::Texture::Import(imageUri, specularTexture);
                    rMaterial->mSpecularGlossinessTexture = specularTexture;

                    rMaterial->mEnableSpecularGlossinessTexture = true;

                    if (specularTexture->HasAlpha()) {
                        rMaterial->mEnableShinessMap = true;
                    }
                }
            }
        }

        if (extensionMap.Has("normalTexture"))
        {
            const tinygltf::Value& normalTexture = extensionMap.Get("normalTexture");

            if (normalTexture.IsObject()) {
                const tinygltf::Value& indexValue = normalTexture.Get("index");

                if (indexValue.IsInt()) {
                    int normalIndex = indexValue.Get<int>();
                    const tinygltf::Texture& normalMap = model.textures[normalIndex];
                    const tinygltf::Image& image = model.images[normalMap.source];
                    const char* imageUri = image.uri.c_str();

                    ResourceTexture* normalTexture = new ResourceTexture();
                    normalTexture->mTextureName = imageUri;
                    Importer::Texture::Import(imageUri, normalTexture);
                    rMaterial->mNormalTexture = normalTexture;

                    rMaterial->mEnableNormalMap = true;
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

            ResourceTexture* diffuseTexture = new ResourceTexture();
            diffuseTexture->mTextureName = imageUri;
            Importer::Texture::Import(imageUri, diffuseTexture);
            rMaterial->mDiffuseTexture = diffuseTexture;

            rMaterial->mEnableDiffuseTexture = true;
        }

    }
}


