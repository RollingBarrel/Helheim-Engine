#include "ResourceTexture.h"
#include "SaveLoadTexture.h"

#include <string>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "glew.h"

#include "DirectXTex.h"
#undef max


ResourceTexture::ResourceTexture(
    unsigned int uid,
    unsigned int glTarget,
    unsigned int width,
    unsigned int height,
    unsigned int internalFormat,
    unsigned int texFormat,
    unsigned int dataType,
    unsigned int mipLevels,
    unsigned int pixelsSize,
    bool hasAlpha,
    unsigned int texelSize): Resource(uid, Type::Texture), 
    mGLTarget(glTarget), mWidth(width), mHeight(height),
    mInternalFormat(internalFormat), mTexFormat(texFormat), mDataType(dataType), 
    mMipLevels(mipLevels),
    mPixelsSize(pixelsSize), 
    mTexelSize(texelSize),
    mHasAlpha(hasAlpha), 
    mOpenGLId(0),
    mTexHandle(0)
{
    // Generate OpenGL texture
    glGenTextures(1, &mOpenGLId);
}

ResourceTexture::~ResourceTexture()
{
    if (mTexHandle)
        glMakeTextureHandleNonResidentARB(mTexHandle);
    glDeleteTextures(1, &mOpenGLId);
    mOpenGLId = 0;
    mTexHandle = 0;
}

bool ResourceTexture::IsCubemap() const 
{ 
    return mGLTarget == GL_TEXTURE_CUBE_MAP;
}

void ResourceTexture::MakeTextutureBindless()
{
    if (mTexHandle == 0)
    {
        mTexHandle = glGetTextureHandleARB(mOpenGLId);
        glMakeTextureHandleResidentARB(mTexHandle);
    }
}

void ResourceTexture::GenerateMipmaps()
{
    if (mMipLevels == 1)
    {
        glBindTexture(mGLTarget, mOpenGLId);
        mMipLevels = log2(std::max(mWidth, mHeight));
        glTexParameteri(mGLTarget, GL_TEXTURE_MAX_LOD, mMipLevels - 1);
        glTexParameteri(mGLTarget, GL_TEXTURE_MAX_LEVEL, mMipLevels - 1);
        glTexParameteri(mGLTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glGenerateMipmap(mGLTarget);

        unsigned int w = mWidth/2;
        unsigned int h = mHeight/2;
        for (unsigned int i = 1; i < mMipLevels; ++i)
        {
            mPixelsSize += w * h * mTexelSize;
            if(w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }
        unsigned char* pixels = new unsigned char[mPixelsSize];
        unsigned char* cursor = pixels;
        w = mWidth;
        h = mHeight;
        for (unsigned int i = 0; i < mMipLevels; ++i)
        {
            glGetTexImage(mGLTarget, i, mTexFormat, mDataType, cursor);
            cursor += w * h * mTexelSize;
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }
        glBindTexture(mGLTarget, 0);
        Importer::Texture::Save(this, pixels);
        delete[] pixels;
    }
}