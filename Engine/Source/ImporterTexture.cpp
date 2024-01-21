#include <string>

#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterTexture.h"

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
        texture->mFormat = GL_RGBA;
        texture->mType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        texture->mInternalFormat = GL_RGBA8;
        texture->mFormat = GL_BGRA;
        texture->mType = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        texture->mInternalFormat = GL_RGB8;
        texture->mFormat = GL_BGR;
        texture->mType = GL_UNSIGNED_BYTE;
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
    unsigned int header[7] = { texture->mWidth, texture->mHeight, texture->mInternalFormat, texture->mFormat, texture->mType ,texture->mMipLevels, texture->mNumPixels};

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
    std::string path = LIBRARY_TEXTURE_PATH;
    path += std::to_string(texture->mUID);
    path += ".textssy";

    App->GetFileSystem()->Save(path.c_str(), fileBuffer, size);

    delete[] fileBuffer;
    fileBuffer = nullptr;

}

unsigned int Importer::Texture::Load(ResourceTexture* texture, const char* fileName)
{
    char* fileBuffer;

    std::string path = LIBRARY_TEXTURE_PATH;
    path += fileName;
    path += ".textssy";

    App->GetFileSystem()->Load(path.c_str(), &fileBuffer);

    char* cursor = fileBuffer;
    unsigned int header[7];
    unsigned int bytes = sizeof(header);
    memcpy(header, cursor, bytes);
    cursor += bytes;
    texture->mWidth = header[0];
    texture->mHeight = header[1];
    texture->mInternalFormat = header[2];
    texture->mFormat = header[3];
    texture->mType = header[4];
    texture->mMipLevels = header[5];
    texture->mNumPixels = header[6];

    bytes = sizeof(texture->mHasAlpha);
    memcpy(&texture->mHasAlpha, cursor, bytes);
    cursor += bytes;

    bytes = sizeof(unsigned char) * texture->mNumPixels;
    if (texture->mPixels != nullptr)
        delete[] texture->mPixels;
    texture->mPixels = new unsigned char[texture->mNumPixels];
    memcpy(cursor, texture->mPixels, bytes);
    cursor += bytes;

    return texture->CreateTexture();
}

unsigned int ResourceTexture::CreateTexture()
{
    unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	for (size_t i = 0; i < mMipLevels; ++i)
	{
		glTexImage2D(GL_TEXTURE_2D, i, mInternalFormat, mWidth, mHeight, 0, mFormat, mType, mPixels);
	}

	if (mMipLevels == 1)
		glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mMipLevels - 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);

    openGlId = texId;
	return texId;
}
