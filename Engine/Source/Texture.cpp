#include "Texture.h"
#include "Application.h"
#include "DirectXTex.h" 
#include "glew.h" 

#include <string>
Texture::Texture()
{
}

Texture::~Texture()
{
}
void Texture::LoadTexture(const char* filePath) {

    HRESULT result = E_FAIL;
    DirectX::ScratchImage image;

    const char* prefix = "models/Clock/";
    size_t prefixLength = strlen(prefix);
    size_t filePathLength = strlen(filePath);

    size_t newSize = prefixLength + filePathLength + 1;
    wchar_t* pathTex = new wchar_t[newSize];

    size_t outSize;
    mbstowcs_s(&outSize, pathTex, prefixLength + 1, prefix, prefixLength);

    mbstowcs_s(&outSize, pathTex + prefixLength, filePathLength + 1, filePath, filePathLength);

    result = LoadFromDDSFile(pathTex, DirectX::DDS_FLAGS_NONE, nullptr, image);
    if (result != S_OK) {

        result = LoadFromTGAFile(pathTex, DirectX::TGA_FLAGS_NONE, nullptr, image);

        if (result != S_OK) {
            result = LoadFromWICFile(pathTex, DirectX::WIC_FLAGS_NONE, nullptr, image);
        }
    }
    assert(result == S_OK);
    unsigned textureId = 0u;
    //2. Create and load OpenGL texture into GPU
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glObjectLabel(GL_TEXTURE, textureId, -1, filePath);
    //2.1 Loading texture data to OpenGL
    GLint internalFormat = 0;
    GLsizei imWidth = image.GetMetadata().width;
    GLsizei imHeigh = image.GetMetadata().height;
    GLenum format = 0;
    GLenum  type = 0;

    switch (image.GetMetadata().format)
    {
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
        case DXGI_FORMAT_R8G8B8A8_UNORM:
            internalFormat = GL_RGBA8;
            format = GL_RGBA;
            type = GL_UNSIGNED_BYTE;
            break;
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
        case DXGI_FORMAT_B8G8R8A8_UNORM:
            internalFormat = GL_RGBA8;
            format = GL_BGRA;
            type = GL_UNSIGNED_BYTE;
            break;
        case DXGI_FORMAT_B5G6R5_UNORM:
            internalFormat = GL_RGB8;
            format = GL_BGR;
            type = GL_UNSIGNED_BYTE;
            break;
        default:
            assert(false && "Unsupported format");
    }

    // if ScratchImage contains mipmaps, we can load them into Video RAM by calling glTexImage2D several times
    if (image.GetMetadata().mipLevels > 0) {
        for (size_t i = 0; i < image.GetMetadata().mipLevels; ++i)
        {
            const DirectX::Image* mip = image.GetImage(i, 0, 0);
            glTexImage2D(GL_TEXTURE_2D, i, internalFormat, mip->width, mip->height, 0, format, type, mip->pixels);

        }
    }
    else {
        void* imData = image.GetPixels();
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
            imWidth, imHeigh, 0,
            format, type, imData
        );
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // Deformation when scale
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, image.GetMetadata().mipLevels - 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    mTextureID = textureId;
    mImageWidth = imWidth;
    mImageHeight = imHeigh;
}
