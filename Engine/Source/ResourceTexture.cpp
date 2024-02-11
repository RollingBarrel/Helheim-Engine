#include "ResourceTexture.h"

#include <string>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "glew.h"

#include "DirectXTex.h"


ResourceTexture::ResourceTexture()
{
    mWidth = 0;
    mHeight = 0;

    mInternalFormat = 0;
    mTexFormat = 0;
    mDataType = 0;
    mMipLevels = 0;
    mNumPixels = 0;
    mPixels = nullptr;

    mHasAlpha = false;

    mOpenGLId = 0;
}

ResourceTexture::ResourceTexture(unsigned int id) : Resource(mUID)
{
    mWidth = 0;
    mHeight = 0;

    mInternalFormat = 0;
    mTexFormat = 0;
    mDataType = 0;
    mMipLevels = 0;
    mNumPixels = 0;
    mPixels = nullptr;

    mHasAlpha = false;

    mOpenGLId = 0;
}

ResourceTexture::~ResourceTexture()
{
}


void ResourceTexture::Import(const char* filePath)
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
        mInternalFormat = GL_RGBA8;
        mTexFormat = GL_RGBA;
        mDataType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        mInternalFormat = GL_RGBA8;
        mTexFormat = GL_BGRA;
        mDataType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        mInternalFormat = GL_RGB8;
        mTexFormat = GL_BGR;
        mDataType = GL_UNSIGNED_BYTE;
        break;
    default:
        assert(false && "Unsupported format");
    }

    mWidth = image.GetMetadata().width;
    mHeight = image.GetMetadata().height;
    mMipLevels = image.GetMetadata().mipLevels;
    mNumPixels = image.GetPixelsSize();

    mPixels = new unsigned char[mNumPixels];
    for (auto i = 0; i < image.GetPixelsSize(); ++i)
    {
        mPixels[i] = image.GetPixels()[i];
    }

    if (DirectX::HasAlpha(image.GetMetadata().format))
    {
        mHasAlpha = true;
    }
}

void ResourceTexture::Save()
{
    unsigned int header[7] = { mWidth, mHeight, mInternalFormat, mTexFormat, mDataType ,mMipLevels, mNumPixels };

    unsigned int size = sizeof(header) +
        sizeof(mHasAlpha) +
        sizeof(unsigned char) * mNumPixels;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    bytes = sizeof(mHasAlpha);
    memcpy(cursor, &mHasAlpha, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * mNumPixels;
    memcpy(cursor, mPixels, bytes);
    cursor += bytes;

    //TODO Change name for random UID
    //std::string path = LIBRARY_TEXTURE_PATH;
    //path += std::to_string(GetUID());
    //path += ".tex";

    App->GetFileSystem()->Save(mLibraryFile.c_str(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;

}

unsigned int ResourceTexture::Load(const char* fileName)
{
    char* fileBuffer;

    //std::string path = LIBRARY_TEXTURE_PATH;
    //path += fileName;
    //path += ".tex";

    App->GetFileSystem()->Load(mLibraryFile.c_str(), &fileBuffer);

    char* cursor = fileBuffer;
    unsigned int header[7];
    unsigned int bytes = sizeof(header);
    memcpy(header, cursor, bytes);
    cursor += bytes;
    mWidth = header[0];
    mHeight = header[1];
    mInternalFormat = header[2];
    mTexFormat = header[3];
    mDataType = header[4];
    mMipLevels = header[5];
    mNumPixels = header[6];

    bytes = sizeof(mHasAlpha);
    memcpy(&mHasAlpha, cursor, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * mNumPixels;
    if (mPixels != nullptr)
        delete[] mPixels;
    mPixels = new unsigned char[mNumPixels];
    memcpy(mPixels, cursor, bytes);

    return CreateTexture();
}


unsigned int ResourceTexture::CreateTexture()
{
    unsigned int texId;

    // Generate OpenGL texture
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    // Set texture data for each mip level
    for (size_t i = 0; i < mMipLevels; ++i)
    {
        glTexImage2D(GL_TEXTURE_2D, i, mInternalFormat, mWidth, mHeight, 0, mTexFormat, mDataType, mPixels);
    }

    // Generate mipmaps if only one mip level is present
    if (mMipLevels == 1)
        glGenerateMipmap(GL_TEXTURE_2D);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mMipLevels - 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D, 0);

    // Store OpenGL texture ID
    mOpenGLId = texId;

    return texId;
}
