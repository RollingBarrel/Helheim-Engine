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
    bool hasAlpha,
    unsigned int openGLId,
    unsigned int texHandle)
    : Resource(uid, Type::Texture), mWidth(width), mHeight(height),
    mInternalFormat(internalFormat), mTexFormat(texFormat), mDataType(dataType), 
    mMipLevels(mipLevels),
    mNumPixels(numPixels), 
    mHasAlpha(hasAlpha), 
    mOpenGLId(openGLId),
    mTexHandle(texHandle)
{
}

ResourceTexture::~ResourceTexture()
{
    if (mTexHandle)
        glMakeTextureHandleNonResidentARB(mTexHandle);
    if (mOpenGLId)
        glDeleteTextures(1, &mOpenGLId);
    mOpenGLId = 0;
    mTexHandle = 0;
}