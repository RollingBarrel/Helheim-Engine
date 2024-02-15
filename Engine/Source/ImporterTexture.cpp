#include <string>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterTexture.h"
#include "ResourceTexture.h"

#include "glew.h"

#include "DirectXTex.h"


ResourceTexture* Importer::Texture::Import(const char* filePath, unsigned int uid)
{
    std::string gltfPath = (ASSETS_TEXTURE_PATH + std::string(filePath));

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
                return;
            }
        }
    }
    unsigned int internalFormat;
    unsigned int texFormat;
    unsigned int dataType;

    switch (image.GetMetadata().format) // for get all information of the texture and see the parameters it have
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
    default:
        assert(false && "Unsupported format");
    }

    unsigned int width = image.GetMetadata().width;
    unsigned int height = image.GetMetadata().height;
    unsigned int mipLevels = image.GetMetadata().mipLevels;
    unsigned int numPixels = image.GetPixelsSize();

    unsigned char* pixels = new unsigned char[numPixels];
    for (auto i = 0; i < image.GetPixelsSize(); ++i)
    {
        pixels[i] = image.GetPixels()[i];
    }
    bool hasAlpha = false;
    if (DirectX::HasAlpha(image.GetMetadata().format))
    {
        hasAlpha = true;
    }

    return new ResourceTexture(uid, filePath, width, height, internalFormat, texFormat, dataType, mipLevels, numPixels, pixels, hasAlpha);
}

void Importer::Texture::Save(const ResourceTexture* texture)
{
    // TODO: Save also UID (and assetsFile?)
    unsigned int header[7] = { texture->GetWidth(), texture->GetHeight(), texture->GetInternalFormat(), texture->GetTexFormat(), texture->GetDataType() ,texture->GetMipLevels(), texture-> GetNumPixels()};

    unsigned int numPixels = texture->GetNumPixels();
    bool hasAlpha = texture->HasAlpha();
    unsigned int size = sizeof(header) +
                        sizeof(hasAlpha) +
                        sizeof(unsigned char) * numPixels;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    bytes = sizeof(hasAlpha);
    memcpy(cursor, &hasAlpha, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * numPixels;
    memcpy(cursor, texture->GetPixels(), bytes);
    cursor += bytes;

    //TODO Change name for random UID
    //std::string path = LIBRARY_TEXTURE_PATH;
    //path += std::to_string(texture->GetUID());
    //path += ".tex";

    App->GetFileSystem()->Save(texture->GetLibraryFile().c_str(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;

}

unsigned int Importer::Texture::Load( const char* fileName)
{
    char* fileBuffer;

    // TODO This is in the Resource as mLibraryPath
    std::string path = LIBRARY_TEXTURE_PATH;
    path += fileName;
    path += ".tex";

    App->GetFileSystem()->Load(path.c_str(), &fileBuffer);

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

    unsigned char* pixels;
    bytes = sizeof(unsigned char) * numPixels;
    if (pixels != nullptr)
        delete[] pixels;
    pixels = new unsigned char[numPixels];
    memcpy(pixels, cursor, bytes);

    ResourceTexture* texture = new ResourceTexture(uid, filePath, width, height, internalFormat, texFormat, dataType, mipLevels, numPixels, pixels, hasAlpha);
    unsigned int texId = texture->CreateTexture();
    
    return texId;
}
