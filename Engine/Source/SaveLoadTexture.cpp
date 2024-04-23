#include "SaveLoadTexture.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "glew.h"

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
    unsigned int size = sizeof(header) + sizeof(hasAlpha) + sizeof(unsigned char) * numPixels;

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
            glTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0, texFormat, dataType, pixels);
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