#include "ResourceTexture.h"

#include <string>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "glew.h"

#include "DirectXTex.h"


ResourceTexture::ResourceTexture()
{
}

ResourceTexture::ResourceTexture(unsigned int uid) : Resource(uid)
{
}

ResourceTexture::~ResourceTexture()
{
}

unsigned int ResourceTexture::CreateTexture()
{
    unsigned int texId;
    glGenTextures(1, &texId);
    glBindTexture(GL_TEXTURE_2D, texId);
    for (size_t i = 0; i < mMipLevels; ++i)
    {
        glTexImage2D(GL_TEXTURE_2D, i, mInternalFormat, mWidth, mHeight, 0, mTexFormat, mDataType, mPixels);
    }

    if (mMipLevels == 1)
        glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mMipLevels - 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    mOpenGLId = texId;
    return texId;
}
