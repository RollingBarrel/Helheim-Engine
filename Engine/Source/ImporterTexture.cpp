#include <string>

#include "Application.h"
#include "ModuleFileSystem.h"

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
        LOG("R8G8B8A8_UNORM");
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        internalFormat = GL_RGBA8;
        texFormat = GL_BGRA;
        dataType = GL_UNSIGNED_BYTE;
        LOG("B8G8R8A8_UNORM");
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        internalFormat = GL_RGB8;
        texFormat = GL_BGR;
        dataType = GL_UNSIGNED_BYTE;
        LOG("B5G6R5_UNORM");
        break;
    case DXGI_FORMAT_R16G16B16A16_UNORM:
        internalFormat = GL_RGBA16;
        texFormat = GL_RGBA;
        dataType = GL_UNSIGNED_SHORT;
        LOG("R16G16B16A16_UNORM");
        break;
    case DXGI_FORMAT_BC1_UNORM:
        //internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT;
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
        LOG("BC1");
        break;
    case DXGI_FORMAT_BC3_UNORM:
        //internalFormat = GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT
        internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
        LOG("BC3");
        break;
    case DXGI_FORMAT_BC5_UNORM:
		internalFormat = GL_COMPRESSED_RG_RGTC2;
        //internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
        LOG("BC5");
		break;
    case DXGI_FORMAT_BC7_UNORM:
        internalFormat = GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM;
        LOG("BC7");
        break;
    default:
        assert(false && "Unsupported format");
    }

    ResourceTexture* rTex = new ResourceTexture(uid, width, height, internalFormat, texFormat, dataType, mipLevels, numPixels, hasAlpha, 0, 0);
    Importer::Texture::Save(rTex, image.GetPixels());
    return rTex;
}

void Importer::Texture::Save(const ResourceTexture* rTexture, const unsigned char* pixelData)
{
    unsigned int header[7] = {
        rTexture->GetWidth(),
        rTexture->GetHeight(),
        rTexture->GetInternalFormat(),
        rTexture->GetTexFormat(),
        rTexture->GetDataType(),
        rTexture->GetMipLevels(),
        rTexture->GetNumPixels()
    };

    unsigned int numPixels = rTexture->GetNumPixels();
    bool hasAlpha = rTexture->HasAlpha();
    unsigned int size = sizeof(header)  + sizeof(hasAlpha) + sizeof(unsigned char) * numPixels;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    bytes = sizeof(hasAlpha);
    memcpy(cursor, &hasAlpha, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * numPixels;
    memcpy(cursor, pixelData, bytes);
    cursor += bytes;

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(rTexture->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceTexture* Importer::Texture::Load(const char* filePath, unsigned int uid)
{
    char* fileBuffer;
    ResourceTexture* texture = nullptr;

    if (App->GetFileSystem()->Load(filePath, &fileBuffer))
    {
        char* cursor = fileBuffer;
        unsigned int header[7];
        unsigned int bytes = sizeof(header);
        memcpy(header, cursor, bytes);
        cursor += bytes;
        unsigned int width = header[0];
        unsigned int height = header[1];
        unsigned int internalFormat = header[2];
        unsigned int texFormat = header[3];
        unsigned int dataType = header[4];
        unsigned int mipLevels = header[5];
        unsigned int numPixels = header[6];

        bool hasAlpha;
        bytes = sizeof(hasAlpha);
        memcpy(&hasAlpha, cursor, bytes);
        cursor += bytes;

        unsigned char* pixels = reinterpret_cast<unsigned char*>(cursor);

        //Load the texture to memory
        unsigned int openGLId = 0;
        unsigned int texHandle = 0;
        // Generate OpenGL texture
        glGenTextures(1, &openGLId);
        glBindTexture(GL_TEXTURE_2D, openGLId);

        // Set texture data for each mip level
        for (size_t i = 0; i < mipLevels; ++i)
        {
            if (internalFormat == GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT || internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT3_EXT || internalFormat == GL_COMPRESSED_RG_RGTC2 || internalFormat == GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM)
            {
				glCompressedTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0, numPixels, pixels);
			}
            else
            {
				glTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0, texFormat, dataType, pixels);
			}
        }

        // Generate mipmaps if only one mip level is present
        if (mipLevels == 1)
            glGenerateMipmap(GL_TEXTURE_2D);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Store OpenGL texture handle ID
        texHandle = glGetTextureHandleARB(openGLId);
        glMakeTextureHandleResidentARB(texHandle);

        texture = new ResourceTexture(uid, width, height, internalFormat, texFormat, dataType, mipLevels, numPixels, hasAlpha, openGLId, texHandle);

        delete[] fileBuffer;
    }
    
    return texture;
}

// Check if a string contains the text
bool Importer::Texture::ContainsText(const std::string& str, const std::string& text)
{
    return str.find(text) != std::string::npos;
}

const std::unordered_map<std::string, DXGI_FORMAT> Importer::Texture::compressionFormatNaming = {
    {"_BaseColor", DXGI_FORMAT_BC1_UNORM},
    //{"_Normal", DXGI_FORMAT_BC5_UNORM},
    {"_Occlusion", DXGI_FORMAT_BC5_UNORM},
    {"_Roughness", DXGI_FORMAT_BC5_UNORM},
    {"_Metallic", DXGI_FORMAT_BC5_UNORM},
    {"_Emissive", DXGI_FORMAT_BC5_UNORM}
};