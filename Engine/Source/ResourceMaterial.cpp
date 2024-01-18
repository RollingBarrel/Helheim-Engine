#include "ResourceMaterial.h"
#include "Texture.h"

#include "tiny_gltf.h"

ResourceMaterial::ResourceMaterial()
    : mDiffuseFactor(1.0f, 1.0f, 1.0f, 1.0f),
    mSpecularFactor(0.0f, 0.0f, 0.0f),
    mGlossinessFactor(1.0f),
    mDiffuseTexture(nullptr),
    mSpecularGlossinessTexture(nullptr)
{

}

ResourceMaterial::~ResourceMaterial()
{
}

void ResourceMaterial::LoadMaterial(const tinygltf::Model& model, const tinygltf::Material& material)
{
    auto it = material.extensions.find("KHR_materials_pbrSpecularGlossiness");
    if (it != material.extensions.end()) {
        const tinygltf::Value& extensionMap = it->second;

        if (extensionMap.Has("diffuseFactor")) {
            const tinygltf::Value& diffuseFactor = extensionMap.Get("diffuseFactor");
            if (diffuseFactor.IsArray()) {
                for (int i = 0; i < 4; ++i) {
                    mDiffuseFactor[i] = static_cast<float>(diffuseFactor.Get(i).Get<double>());
                }
            }
        }

        if (extensionMap.Has("specularFactor")) {
            const tinygltf::Value& specularFactor = extensionMap.Get("specularFactor");
            if (specularFactor.IsArray()) {
                for (int i = 0; i < 3; ++i) {
                    mSpecularFactor[i] = static_cast<float>(specularFactor.Get(i).Get<double>());
                }
            }
        }

        if (extensionMap.Has("glossinessFactor")) {
            const tinygltf::Value& glossinessFactor = extensionMap.Get("glossinessFactor");
            if (glossinessFactor.IsNumber()) {
                mGlossinessFactor = static_cast<float>(glossinessFactor.Get<double>());
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

                    Texture* diffuseTexture = new Texture();
                    diffuseTexture->LoadTexture(imageUri);
                    mDiffuseTexture = diffuseTexture;
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

                    Texture* specularTexture = new Texture();
                    specularTexture->LoadTexture(imageUri);
                    mSpecularGlossinessTexture = specularTexture;
                }
            }
        }

    }
}