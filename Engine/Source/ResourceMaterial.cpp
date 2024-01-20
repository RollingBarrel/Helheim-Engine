#include "ResourceMaterial.h"
#include "Texture.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

ResourceMaterial::ResourceMaterial()
    : mDiffuseFactor(1.0f, 1.0f, 1.0f, 1.0f),
    mSpecularFactor(0.0f, 0.0f, 0.0f),
    mGlossinessFactor(1.0f),
    mDiffuseTexture(nullptr),
    mSpecularGlossinessTexture(nullptr),
    mNormalTexture(nullptr),
    mEnableDiffuseTexture(false),
    mEnableSpecularGlossinessTexture(false),
    mEnableNormalMap(false),
    mEnableShinessMap(false)
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

                    Texture* diffuseTexture = new Texture(mTemporalID);
                    diffuseTexture->LoadTexture(imageUri);
                    mDiffuseTexture = diffuseTexture;

                    mEnableDiffuseTexture = true;
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

                    Texture* specularTexture = new Texture(mTemporalID);
                    specularTexture->LoadTexture(imageUri);
                    mSpecularGlossinessTexture = specularTexture;

                    mEnableSpecularGlossinessTexture = true;

                    if (specularTexture->hasAlpha()) {
                        mEnableShinessMap = true;
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

                    Texture* normalTexture = new Texture(mTemporalID);
                    normalTexture->LoadTexture(imageUri);
                    mNormalTexture = normalTexture;

                    mEnableNormalMap = true;
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

            Texture* diffuseTexture = new Texture(mTemporalID);
            diffuseTexture->LoadTexture(imageUri);
            mDiffuseTexture = diffuseTexture;

            mEnableDiffuseTexture = true;
        }

    }
}