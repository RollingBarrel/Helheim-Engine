#include "Globals.h"
#include "SaveLoadTexture.h"

#include "ImporterTexture.h"
#include "ResourceTexture.h"
#include "glew.h"

#include "DirectXTex.h"

ResourceTexture* Importer::Texture::Import(const char* filePath, unsigned int uid)
{
    std::string gltfPath = (filePath);

    DirectX::ScratchImage image;

    size_t size = strlen(gltfPath.c_str()) + 1;
    wchar_t* pathTex = new wchar_t[size];
    size_t outSize;
    mbstowcs_s(&outSize, pathTex, size, gltfPath.c_str(), size - 1);

    // every texture have one format, if the format its not the correct it use another method for load
    HRESULT hr = DirectX::LoadFromDDSFile(pathTex, DirectX::DDS_FLAGS_NONE, nullptr, image);
    if (FAILED(hr))
    {
        hr = DirectX::LoadFromTGAFile(pathTex, DirectX::TGA_FLAGS_NONE, nullptr, image);

        if (FAILED(hr))
        {
            hr = DirectX::LoadFromWICFile(pathTex, DirectX::WIC_FLAGS_NONE, nullptr, image);

            if (FAILED(hr))
            {
                LOG("texture failed to load");
                return nullptr;
            }
        }
    }
    
    delete[] pathTex;

    // for get all information of the texture and see the parameters it have
    unsigned int internalFormat;
    unsigned int texFormat;
    unsigned int dataType;

    switch (image.GetMetadata().format) 
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        internalFormat = GL_RGBA8;
        texFormat = GL_RGBA;
        dataType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        internalFormat = GL_RGBA8;
        texFormat = GL_BGRA;
        dataType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        internalFormat = GL_RGB8;
        texFormat = GL_BGR;
        dataType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        internalFormat = GL_RGBA16;
        texFormat = GL_RGBA;
        dataType = GL_UNSIGNED_SHORT;
        break;
    default:
        assert(false && "Unsupported format");
    }

    unsigned int width = image.GetMetadata().width;
    unsigned int height = image.GetMetadata().height;
    unsigned int mipLevels = image.GetMetadata().mipLevels;
    unsigned int numPixels = image.GetPixelsSize();

    bool hasAlpha = false;
    if (DirectX::HasAlpha(image.GetMetadata().format))
    {
        hasAlpha = true;
    }

    ResourceTexture* rTex = new ResourceTexture(uid, width, height, internalFormat, texFormat, dataType, mipLevels, numPixels, hasAlpha, 0, 0);
    Importer::Texture::Save(rTex, image.GetPixels());
    return rTex;
}