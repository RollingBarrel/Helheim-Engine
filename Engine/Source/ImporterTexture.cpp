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
                LOG("Texture failed to load");
                return nullptr;
            }
        }
    }
    
    delete[] pathTex;

    DXGI_FORMAT compressionFormat = DXGI_FORMAT_BC3_UNORM; // Default value
    DirectX::ScratchImage compressedImage;

    // Determine the compression format (if needed) based on the file name
    for (const auto& pair : Importer::Texture::compressionFormatNaming)
    {
        if (ContainsText(filePath, pair.first))
        {
            compressionFormat = pair.second;

            // If the texture is a BaseColor and has alpha, use BC3 instead of BC1 to avoid artifacts
            if (compressionFormat == DXGI_FORMAT_BC1_UNORM && DirectX::HasAlpha(image.GetMetadata().format))
            {
                compressionFormat = DXGI_FORMAT_BC3_UNORM;
            }
            hr = DirectX::Compress(image.GetImages(), image.GetImageCount(), image.GetMetadata(), compressionFormat, DirectX::TEX_COMPRESS_DEFAULT, 0.5f, compressedImage);
            if (FAILED(hr))
            {
                LOG("Failed to compress texture");
                return nullptr;
            }

            // Replaces the original image with the compressed one
            image = std::move(compressedImage);
            break;
        }
    }

    // For get all information of the texture and see the parameters it have
    unsigned int internalFormat = 0;
    unsigned int texFormat = 0;
    unsigned int dataType = 0;
    unsigned int width = image.GetMetadata().width;
    unsigned int height = image.GetMetadata().height;
    unsigned int mipLevels = image.GetMetadata().mipLevels;
    unsigned int numPixels = image.GetPixelsSize();
    bool hasAlpha = false;

    if (DirectX::HasAlpha(image.GetMetadata().format))
    {
        hasAlpha = true;
    }

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
    case DXGI_FORMAT_BC1_UNORM:
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        texFormat = 0;
        dataType = 0;
        break;
    case DXGI_FORMAT_BC3_UNORM:
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        texFormat = 0;
        dataType = 0;
        break;
    case DXGI_FORMAT_BC5_UNORM:
		internalFormat = GL_COMPRESSED_RG_RGTC2;
        texFormat = 0;
        dataType = 0;
		break;
    case DXGI_FORMAT_BC7_UNORM:
        internalFormat = GL_COMPRESSED_RGBA_BPTC_UNORM;
        texFormat = 0;
        dataType = 0;
        break;
    default:
        assert(false && "Unsupported format");
    }

    ResourceTexture* rTex = new ResourceTexture(uid, width, height, internalFormat, texFormat, dataType, mipLevels, numPixels, hasAlpha, 0, 0);
    Importer::Texture::Save(rTex, image.GetPixels());
    return rTex;
}

// Check if a string contains the text
bool Importer::Texture::ContainsText(const std::string& str, const std::string& text)
{
    return str.find(text) != std::string::npos;
}

const std::unordered_map<std::string, DXGI_FORMAT> Importer::Texture::compressionFormatNaming = {
    {"_BaseColor", DXGI_FORMAT_BC1_UNORM},
    //{"_Normal", DXGI_FORMAT_BC5_UNORM},
    {"_Occlusion", DXGI_FORMAT_BC3_UNORM},
    {"_Roughness", DXGI_FORMAT_BC3_UNORM},
    {"_Metallic", DXGI_FORMAT_BC3_UNORM},
    {"_Emissive", DXGI_FORMAT_BC3_UNORM}
};
