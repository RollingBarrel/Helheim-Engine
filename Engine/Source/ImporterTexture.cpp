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

    switch (image.GetMetadata().format) // for get all information of the texture and see the parameters it have
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        texture->mInternalFormat = GL_RGBA8;
        texture->mTexFormat = GL_RGBA;
        texture->mDataType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        texture->mInternalFormat = GL_RGBA8;
        texture->mTexFormat = GL_BGRA;
        texture->mDataType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        texture->mInternalFormat = GL_RGB8;
        texture->mTexFormat = GL_BGR;
        texture->mDataType = GL_UNSIGNED_BYTE;
        break;
    default:
        assert(false && "Unsupported format");
    }

    texture->mWidth = image.GetMetadata().width;
    texture->mHeight = image.GetMetadata().height;
    texture->mMipLevels = image.GetMetadata().mipLevels;
    texture->mNumPixels = image.GetPixelsSize();

    texture->mPixels = new unsigned char[texture->mNumPixels];
    for (auto i = 0; i < image.GetPixelsSize(); ++i)
    {
        texture->mPixels[i] = image.GetPixels()[i];
    }

    if (DirectX::HasAlpha(image.GetMetadata().format))
    {
        texture->mHasAlpha = true;
    }

    Texture::Save(texture);
}

void Importer::Texture::Save(const ResourceTexture* texture)
{
    unsigned int header[7] = { texture->mWidth, texture->mHeight, texture->mInternalFormat, texture->mTexFormat, texture->mDataType ,texture->mMipLevels, texture->mNumPixels};

    unsigned int size = sizeof(header) +
                        sizeof(texture->mHasAlpha) +
                        sizeof(unsigned char) * texture->mNumPixels;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    bytes = sizeof(texture->mHasAlpha);
    memcpy(cursor, &texture->mHasAlpha, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * texture->mNumPixels;
    memcpy(cursor, texture->mPixels, bytes);
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
    texture->mWidth = header[0];
    texture->mHeight = header[1];
    texture->mInternalFormat = header[2];
    texture->mTexFormat = header[3];
    texture->mDataType = header[4];
    texture->mMipLevels = header[5];
    texture->mNumPixels = header[6];

    bytes = sizeof(texture->mHasAlpha);
    memcpy(&texture->mHasAlpha, cursor, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * texture->mNumPixels;
    if (texture->mPixels != nullptr)
        delete[] texture->mPixels;
    texture->mPixels = new unsigned char[texture->mNumPixels];
    memcpy(texture->mPixels, cursor, bytes);

    return texture->CreateTexture();
}
