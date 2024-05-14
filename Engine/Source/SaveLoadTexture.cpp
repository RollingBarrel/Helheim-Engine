#include "SaveLoadTexture.h"
#include "ResourceTexture.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "glew.h"

void Importer::Texture::Save(const ResourceTexture* rTexture, const unsigned char* pixelData)
{
    unsigned int header[9] = {
        rTexture->GetTextureGLTarget(),
        rTexture->GetWidth(),
        rTexture->GetHeight(),
        rTexture->GetInternalFormat(),
        rTexture->GetTexFormat(),
        rTexture->GetDataType(),
        rTexture->GetMipLevels(),
        rTexture->GetPixelsSize(),
        rTexture->GetTexelSize()
    };


    unsigned int pixelsSize = rTexture->GetPixelsSize();
    bool hasAlpha = rTexture->HasAlpha();
    unsigned int size = sizeof(header) + sizeof(hasAlpha) + sizeof(unsigned char) * pixelsSize;

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    bytes = sizeof(hasAlpha);
    memcpy(cursor, &hasAlpha, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * pixelsSize;
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
        unsigned int header[9];
        unsigned int bytes = sizeof(header);
        memcpy(header, cursor, bytes);
        cursor += bytes;
        unsigned int glTarget = header[0];
        unsigned int width = header[1];
        unsigned int height = header[2];
        unsigned int internalFormat = header[3];
        unsigned int texFormat = header[4];
        unsigned int dataType = header[5];
        unsigned int mipLevels = header[6];
        unsigned int pixelsSize = header[7];
        unsigned int texelSize = header[8];

        bool hasAlpha;
        bytes = sizeof(hasAlpha);
        memcpy(&hasAlpha, cursor, bytes);
        cursor += bytes;

        unsigned char* pixels = reinterpret_cast<unsigned char*>(cursor);

        texture = new ResourceTexture(uid, glTarget, width, height, internalFormat, texFormat, dataType, mipLevels, pixelsSize, hasAlpha, texelSize);
        glBindTexture(glTarget, texture->GetOpenGLId());

        //TODO: Compression !!
        // Set texture data for each mip level
        //for (size_t i = 0; i < mipLevels; ++i)
        //{
        //    if (internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT || internalFormat == GL_COMPRESSED_RGBA_S3TC_DXT5_EXT || internalFormat == GL_COMPRESSED_RG_RGTC2 || internalFormat == GL_COMPRESSED_RGBA_BPTC_UNORM)
        //    {
        //        glCompressedTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0, numPixels, pixels);
        //    }
        //    else
        //    {
        //        glTexImage2D(GL_TEXTURE_2D, i, internalFormat, width, height, 0, texFormat, dataType, pixels);
        //    }
        //}

        for (size_t i = 0; i < mipLevels; ++i)
        {
            if (glTarget == GL_TEXTURE_2D)
            {
                glTexImage2D(glTarget, i, internalFormat, width, height, 0, texFormat, dataType, pixels);
                pixels += texelSize * width * height;
            }
            if (glTarget == GL_TEXTURE_CUBE_MAP)
            {
                for (unsigned int j = 0; j < 6; ++j)
                {
                    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, internalFormat, width, height, 0, texFormat, dataType, pixels);
                    pixels += texelSize * width * height;
                }
            }
            if(width > 1)
                width /= 2;
            if(height > 1)
                height /= 2;
            //TODO: cubemaps !!!
        }

        //DefaultTextureParameters
        if (mipLevels > 1)
        {
            glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        }
        else
        {
            glTexParameteri(glTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        }
        glTexParameteri(glTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(glTarget, GL_TEXTURE_MIN_LOD, 0);
        glTexParameteri(glTarget, GL_TEXTURE_MAX_LOD, mipLevels - 1);
        glTexParameteri(glTarget, GL_TEXTURE_MAX_LEVEL, mipLevels - 1);

        // Unbind texture
        glBindTexture(glTarget, 0);

        delete[] fileBuffer;
    }

    return texture;
}