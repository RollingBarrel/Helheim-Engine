#include "Application.h"
#include "ModuleFileSystem.h"
#include "SaveLoadIBL.h"
#include "ResourceIBL.h"
#include "glew.h"

static unsigned char* GetOpenglTextureData(unsigned int texId, unsigned int width, unsigned int height, unsigned int mipLevels, unsigned int texelSize, GLenum target, GLenum texFormat, GLenum dataType, unsigned int& dataSize)
{
    dataSize = 0;
    unsigned int w = width;
    unsigned int h = height;
    for (unsigned int i = 0; i < mipLevels; ++i)
    {
        dataSize += ((target == GL_TEXTURE_CUBE_MAP) ? w * h * texelSize * 6 : w * h * texelSize);
        if (w > 1)
            w /= 2;
        if (h > 1)
            h /= 2;
    }
    unsigned char* pixels = new unsigned char[dataSize];
    unsigned char* cursor = pixels;
    w = width;
    h = height;
    glBindTexture(target, texId);
    for (unsigned int i = 0; i < mipLevels; ++i)
    {
        if (target == GL_TEXTURE_CUBE_MAP)
        {
            for (int k = 0; k < 6; ++k)
            {
                glGetTexImage(GL_TEXTURE_CUBE_MAP_POSITIVE_X + k, i, texFormat, dataType, cursor);
                cursor += w * h * texelSize;
            }
        }
        else
        {
            glGetTexImage(target, i, texFormat, dataType, cursor);
            cursor += w * h * texelSize;
        }
        if (w > 1)
            w /= 2;
        if (h > 1)
            h /= 2;
    }
    glBindTexture(target, 0);
    return pixels;
}

void Importer::IBL::Save(const ResourceIBL* resource)
{
    unsigned int environmentTextureDataSize = 0;
    unsigned int numMips = log2(std::max(resource->GetEnvironmentTexSize(), resource->GetEnvironmentTexSize())) + 1;
    unsigned char* environmentTextureData = GetOpenglTextureData(resource->GetEnvironmentTextureId(), resource->GetEnvironmentTexSize(), resource->GetEnvironmentTexSize(), numMips, 6, GL_TEXTURE_CUBE_MAP, GL_RGB, GL_HALF_FLOAT, environmentTextureDataSize);

    unsigned int irradianceTextureDataSize = 0;
    //numMips = log2(std::max(resource->GetIrradianceTexSize(), resource->GetIrradianceTexSize())) + 1;
    numMips = 1;
    unsigned char* irradianceTextureData = GetOpenglTextureData(resource->GetIrradianceTextureId(), resource->GetIrradianceTexSize(), resource->GetIrradianceTexSize(), numMips, 6, GL_TEXTURE_CUBE_MAP, GL_RGB, GL_HALF_FLOAT, irradianceTextureDataSize);

    unsigned int specPrefilteredTextureDataSize = 0;
    numMips = log2(std::max(resource->GetSpecPrefilteredTexSize(), resource->GetSpecPrefilteredTexSize())) + 1;
    unsigned char* specPrefilteredTextureData = GetOpenglTextureData(resource->GetSpecPrefilteredTexId(), resource->GetSpecPrefilteredTexSize(), resource->GetSpecPrefilteredTexSize(), numMips, 6, GL_TEXTURE_CUBE_MAP, GL_RGB, GL_HALF_FLOAT, specPrefilteredTextureDataSize);
    //unsigned int envBRDFTexId = 0;
    unsigned int envBRDFTextureDataSize = 0;
    //numMips = log2(std::max(resource->GetEnvBRDFTexSize(), resource->GetEnvBRDFTexSize())) + 1;
    numMips = 1;
    unsigned char* envBRDFTextureData = GetOpenglTextureData(resource->GetEnvBRDFTexId(), resource->GetEnvBRDFTexSize(), resource->GetEnvBRDFTexSize(), numMips, 4, GL_TEXTURE_2D, GL_RG, GL_HALF_FLOAT, envBRDFTextureDataSize);

    
    const unsigned int size = sizeof(unsigned int) * 4 + environmentTextureDataSize + irradianceTextureDataSize + specPrefilteredTextureDataSize + envBRDFTextureDataSize;
    unsigned char* buffer = new unsigned char[size];
    unsigned char* cursor = buffer;
    *reinterpret_cast<unsigned int*>(cursor) = resource->GetEnvironmentTexSize();
    cursor += sizeof(unsigned int);
    memcpy(cursor, environmentTextureData, environmentTextureDataSize);
    cursor += environmentTextureDataSize;
    *reinterpret_cast<unsigned int*>(cursor) = resource->GetIrradianceTexSize();
    cursor += sizeof(unsigned int);
    memcpy(cursor, irradianceTextureData, irradianceTextureDataSize);
    cursor += irradianceTextureDataSize;
    *reinterpret_cast<unsigned int*>(cursor) = resource->GetSpecPrefilteredTexSize();
    cursor += sizeof(unsigned int);
    memcpy(cursor, specPrefilteredTextureData, specPrefilteredTextureDataSize);
    cursor += specPrefilteredTextureDataSize;
    *reinterpret_cast<unsigned int*>(cursor) = resource->GetEnvBRDFTexSize();
    cursor += sizeof(unsigned int);
    memcpy(cursor, envBRDFTextureData, envBRDFTextureDataSize);
    cursor += envBRDFTextureDataSize;


    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(resource->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, buffer, size);

    delete[] libraryPath;
    delete[] buffer;
}

ResourceIBL* Importer::IBL::Load(const char* filePath, unsigned int uid)
{
    char* fileBuffer;
    ResourceIBL* resource = nullptr;

    if (App->GetFileSystem()->Load(filePath, &fileBuffer))
    {
        char* cursor = fileBuffer;
        unsigned int numMips = 0;
        unsigned int w, h;

        unsigned int environmentSize = *reinterpret_cast<unsigned int*>(cursor);
        cursor += sizeof(unsigned int);
        numMips = log2(std::max(environmentSize, environmentSize)) + 1;
        unsigned int environmentTextureId;
        glGenTextures(1, &environmentTextureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, environmentTextureId);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMips);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        w = environmentSize;
        h = environmentSize;
        for (unsigned int j = 0; j < numMips; ++j)
        {
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, GL_RGB16F, w, h, 0,
                    GL_RGB, GL_HALF_FLOAT, cursor);
                cursor += w * h * 6;
            }
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }


        unsigned int irradianceSize = *reinterpret_cast<unsigned int*>(cursor);
        cursor += sizeof(unsigned int);
        //numMips = log2(std::max(irradianceSize, irradianceSize)) + 1;
        numMips = 1;
        unsigned int irradianceTextureId;
        glGenTextures(1, &irradianceTextureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceTextureId);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMips);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        w = irradianceSize;
        h = irradianceSize;
        for (unsigned int j = 0; j < numMips; ++j)
        {
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, GL_RGB16F, w, h, 0,
                    GL_RGB, GL_HALF_FLOAT, cursor);
                cursor += w * h * 6;
            }
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }

        unsigned int specPrefilteredSize = *reinterpret_cast<unsigned int*>(cursor);
        cursor += sizeof(unsigned int);
        numMips = log2(std::max(specPrefilteredSize, specPrefilteredSize)) + 1;
        unsigned int specPrefilteredTexId;
        glGenTextures(1, &specPrefilteredTexId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, specPrefilteredTexId);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMips);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        w = specPrefilteredSize;
        h = specPrefilteredSize;
        for (unsigned int j = 0; j < numMips; ++j)
        {
            for (unsigned int i = 0; i < 6; ++i)
            {
                glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, j, GL_RGB16F, w, h, 0,
                    GL_RGB, GL_HALF_FLOAT, cursor);
                cursor += w * h * 6;
            }
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }


        unsigned int specEnvBRDFSize = *reinterpret_cast<unsigned int*>(cursor);
        cursor += sizeof(unsigned int);
        //numMips = log2(std::max(specEnvBRDFSize, specEnvBRDFSize)) + 1;
        numMips = 1;
        unsigned int envBRDFTexId;
        glGenTextures(1, &envBRDFTexId);
        glBindTexture(GL_TEXTURE_2D, envBRDFTexId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        w = specEnvBRDFSize;
        h = specEnvBRDFSize;
        for (unsigned int j = 0; j < numMips; ++j)
        {
            glTexImage2D(GL_TEXTURE_2D, j, GL_RG16F, w, h, 0, GL_RG, GL_HALF_FLOAT, cursor);
            cursor += w * h * 4;
            if (w > 1)
                w /= 2;
            if (h > 1)
                h /= 2;
        }

        resource = new ResourceIBL(uid, environmentTextureId, environmentSize, irradianceTextureId, irradianceSize, specPrefilteredTexId, specPrefilteredSize, envBRDFTexId, specEnvBRDFSize);
        delete[] fileBuffer;
    }

    return resource;
}