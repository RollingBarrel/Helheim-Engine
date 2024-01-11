#include "Application.h"
#include "ImporterTexture.h"
#include "DirectXTex.h"
#include "glew.h"

void Importer::Texture::Import(const char* filePath)
{


}

const wchar_t* GetWC(const char* c)
{
    const size_t cSize = strlen(c) + 1;
    size_t zSice;
    wchar_t* wc = new wchar_t[cSize];
    mbstowcs_s(&zSice, wc, cSize, c, cSize - 1);

    return wc;
}

//unsigned solo recoge valores positivos, nunca recoge signo.

void LoadTexture(const char* path, ResourceTex& texture)
{

    DirectX::ScratchImage image;
    const wchar_t* pathTex = GetWC(path);

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
            }
        }
    }

    switch (image.GetMetadata().format) // for get all information of the texture and see the parameters it have
    {
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
        texture.internalFormat = GL_RGBA8;
        texture.format = GL_RGBA;
        texture.type = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
        texture.internalFormat = GL_RGBA8;
        texture.format = GL_BGRA;
        texture.type = GL_UNSIGNED_BYTE;
        break;
    case DXGI_FORMAT_B5G6R5_UNORM:
        texture.internalFormat = GL_RGB8;
        texture.format = GL_BGR;
        texture.type = GL_UNSIGNED_BYTE;
        break;
    default:
        assert(false && "Unsupported format");
    }

    texture.mWidth = image.GetMetadata().width;
    texture.mHeight = image.GetMetadata().height;

    for (auto i = 0; i < image.GetPixelsSize(); ++i)
    {
        texture.pixels[i] = image.GetPixels()[i];
    }

    //unsigned int colorAttachment;
    //glGenTextures(1, &colorAttachment);
    //glBindTexture(GL_TEXTURE_2D, colorAttachment);
    //glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    ////glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight(), 0, GL_RGBA, GL_FLOAT, NULL);
}