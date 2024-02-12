#include <string>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterTexture.h"
#include "ResourceTexture.h"

#include "glew.h"

#include "DirectXTex.h"


void Importer::Texture::Import(const char* filePath, ResourceTexture* texture)
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
        fexFormat = GL_BGRA;
        dataType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        internalFormat = GL_RGB8;
        fexFormat = GL_BGR;
        dataType = GL_UNSIGNED_BYTE;
        break;
    default:
        assert(false && "Unsupported format");
    }

    width = image.GetMetadata().width;
    height = image.GetMetadata().height;
    mipLevels = image.GetMetadata().mipLevels;
    numPixels = image.GetPixelsSize();

    pixels = new unsigned char[numPixels];
    for (auto i = 0; i < image.GetPixelsSize(); ++i)
    {
        pixels[i] = image.GetPixels()[i];
    }

    if (DirectX::HasAlpha(image.GetMetadata().format))
    {
        hasAlpha = true;
    }

    Texture::Save(texture);
}

void Importer::Texture::Save(const ResourceTexture* texture)
{
    unsigned int header[7] = { width, height, internalFormat, fexFormat, dataType ,mipLevels, numPixels};

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
    memcpy(cursor, pixels, bytes);
    cursor += bytes;

    //TODO Change name for random UID
    // TODO This is in the Resource as mLibraryPath
    std::string path = LIBRARY_TEXTURE_PATH;
    path += std::to_string(texture->GetUID());
    path += ".tex";

    App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;

}

unsigned int Importer::Texture::Load(ResourceTexture* texture, const char* fileName)
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
    width = header[0];
    height = header[1];
    internalFormat = header[2];
    fexFormat = header[3];
    dataType = header[4];
    mipLevels = header[5];
    numPixels = header[6];

    bytes = sizeof(hasAlpha);
    memcpy(&hasAlpha, cursor, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * numPixels;
    if (pixels != nullptr)
        delete[] pixels;
    pixels = new unsigned char[numPixels];
    memcpy(pixels, cursor, bytes);

    return texture->CreateTexture();
}
