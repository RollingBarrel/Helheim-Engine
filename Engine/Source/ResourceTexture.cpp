#include "ResourceTexture.h"

#include <string>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "glew.h"

#include "DirectXTex.h"


ResourceTexture::ResourceTexture(
    unsigned int uid,
    unsigned int width,
    unsigned int height,
    unsigned int internalFormat,
    unsigned int texFormat,
    unsigned int dataType,
    unsigned int mipLevels,
    unsigned int numPixels,
    unsigned int libFilePixelOffset,
    bool hasAlpha)
    : Resource(uid, Type::Texture), mWidth(width), mHeight(height),
    mInternalFormat(internalFormat), mTexFormat(texFormat), mDataType(dataType), 
    mMipLevels(mipLevels),
    mNumPixels(numPixels), 
    mLibFilePixelOffset(),
    mHasAlpha(hasAlpha), 
    mOpenGLId(0),
    mTexHandle(0)
{
}

ResourceTexture::~ResourceTexture()
{
    CleanUp();
}

unsigned int ResourceTexture::CreateTexture(const unsigned char* pixels)
{
    const unsigned char* mPixels = nullptr;
    char* fileBuffer = nullptr;
    if (pixels == nullptr)
    {
        //Get the pixel data from file
        const char* path = App->GetFileSystem()->GetLibraryFile(GetUID());
        unsigned int readedBytes = App->GetFileSystem()->Load(path, &fileBuffer);
        if (readedBytes)
        {
            mPixels = reinterpret_cast<unsigned char*>(fileBuffer + mLibFilePixelOffset);
        }
        delete[] path;
        assert(readedBytes && "Error reading texture pixels from file");
    }
    else
        mPixels = pixels;

    CleanUp();
    unsigned int texId;

    // Generate OpenGL texture
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);

    // Set texture data for each mip level
    for (size_t i = 0; i < mMipLevels; ++i)
    {
        glTexImage2D(GL_TEXTURE_2D, i, mInternalFormat, mWidth, mHeight, 0, mTexFormat, mDataType, mPixels);
    }

    if (pixels == nullptr)
        delete[] fileBuffer;

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
    mTexHandle = glGetTextureHandleARB(mOpenGLId);
    glMakeTextureHandleResidentARB(mTexHandle);

    return texId;
}

void ResourceTexture::CleanUp()
{
    if (mTexHandle)
        glMakeTextureHandleNonResidentARB(mTexHandle);
    if(mOpenGLId)
        glDeleteTextures(1, &mOpenGLId);
    mOpenGLId = 0;
    mTexHandle = 0;
}
