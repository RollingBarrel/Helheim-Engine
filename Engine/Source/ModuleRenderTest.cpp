#include "ModuleRenderTest.h"
#include "glew.h"
#include "Application.h"
#include "ModuleOpenGL.h"

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "DirectXTex.h"



ModuleRenderTest::ModuleRenderTest()
{
}

ModuleRenderTest::~ModuleRenderTest()
{
}


static long FileToBuffer(const char* filePath, char** buffer)
{
	//Note: necesito posar tambe el mode de obrir el file??
	FILE* file = fopen(filePath, "rb");
	if (file == NULL)
		return 0;
	fseek(file, 0, SEEK_END);
	long size = ftell(file);
	*buffer = (char*)malloc(size + 1);
	fseek(file, 0, SEEK_SET);
	fread(*buffer, 1, size, file);
	(*buffer)[size] = '\0';
	fclose(file);
	return size;
}

static unsigned int LoadTexture(const char* path)
{
	char* buffer;
	size_t size = FileToBuffer(path, &buffer);
	if (size == 0)
		return 0;
	DirectX::TexMetadata meta;
	DirectX::ScratchImage sImage;
	HRESULT hr = DirectX::LoadFromDDSMemory(buffer, size, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, &meta, sImage);
	if (!SUCCEEDED(hr))
	{
		hr = DirectX::LoadFromTGAMemory(buffer, size, &meta, sImage);
		if (!SUCCEEDED(hr))
		{
			hr = DirectX::LoadFromWICMemory(buffer, size, DirectX::WIC_FLAGS::WIC_FLAGS_NONE, &meta, sImage);
			if (!SUCCEEDED(hr))
			{
				LOG("Error: Could not load the texture from the file");
				return 0;
			}
		}
	}

	int internalFormat;
	int format;
	int type;

	switch (meta.format)
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

	unsigned int texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	for (size_t i = 0; i < meta.mipLevels; ++i)
	{
		const DirectX::Image* mip = sImage.GetImage(i, 0, 0);
		glTexImage2D(GL_TEXTURE_2D, i, internalFormat, meta.width, meta.height, 0, format, type, sImage.GetPixels());
	}

	//TODO: Mipmaps usually not generated when loading the texture??
	if (meta.mipLevels == 1)
		glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, meta.mipLevels - 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texId;
}


static unsigned int CompileShader(const char* sourcePath, GLenum type)
{
	char* sourceBuffer = nullptr;
	unsigned int shaderId = glCreateShader(type);
	if (!FileToBuffer(sourcePath, &sourceBuffer))
	{
		glDeleteShader(shaderId);
		return 0;
	}
	glShaderSource(shaderId, 1, &sourceBuffer, NULL);
	free(sourceBuffer);
	sourceBuffer = nullptr;
	glCompileShader(shaderId);
	int shaderErr = 0;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &shaderErr);
	if (shaderErr == GL_FALSE)
	{
		int length = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
		if (length == 0)
			return 0;
		char* errMessage = (char*)malloc(length);
		glGetShaderInfoLog(shaderId, length, NULL, errMessage);
		LOG("%s\n", errMessage);
		free(errMessage);
		return 0;
	}
	return shaderId;
}


static unsigned int CreateProgram(const char* vShaderPath, const char* fShaderPath)
{
	unsigned int vShaderId = CompileShader(vShaderPath, GL_VERTEX_SHADER);
	if (!vShaderId)
		return 0;
	unsigned int fShaderId = CompileShader(fShaderPath, GL_FRAGMENT_SHADER);
	if (!fShaderId)
	{
		glDeleteShader(vShaderId);
		return 0;
	}
	unsigned int programId = glCreateProgram();
	glAttachShader(programId, vShaderId);
	glAttachShader(programId, fShaderId);
	glLinkProgram(programId);
	int shaderErr = 0;
	glGetProgramiv(programId, GL_LINK_STATUS, &shaderErr);
	if (shaderErr == GL_FALSE)
	{
		int length = 0;
		glGetProgramiv(programId, GL_INFO_LOG_LENGTH, &length);
		if (length == 0)
			return 0;
		char* errMessage = (char*)malloc(length);
		glGetProgramInfoLog(programId, length, NULL, errMessage);
		LOG("%s", errMessage);
		free(errMessage);
		return 0;
	}
	glDetachShader(programId, vShaderId);
	glDetachShader(programId, fShaderId);
	glDeleteShader(vShaderId);
	glDeleteShader(fShaderId);
	return programId;
}

bool ModuleRenderTest::Init()
{
    float vertices[]{
        -1.f, 1.f, 0.f, 0.f, 1.f,
        -1.f, -1.f, 0.f, 0.f, 0.f,
        1.f, -1.f, 0.f, 1.f, 0.f,
        1.f, -1.f, 0.f, 1.f, 0.f,
        1.f, 1.f, 0.f, 1.f, 1.f,
        -1.f, 1.f, 0.f, 0.f, 1.f
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float)*3));
    glBindVertexArray(0);

	programId = CreateProgram("SimpleVertexShader", "SimpleFragmentShader");
	textureId = LoadTexture("brickwall.jpg");

    return true;
}
update_status ModuleRenderTest::Update()
{
    //glBindVertexArray(VAO);
	//glUseProgram(programId);
	//glActiveTexture(0);
	//glBindTexture(GL_TEXTURE_2D, App->GetOpenGL()->GetFramebufferTexture());
	////glBindTexture(GL_TEXTURE_2D, textureId);
    //glDrawArrays(GL_TRIANGLES, 0, 6);
    //glBindVertexArray(0);
    return UPDATE_CONTINUE;
}

//bool ModuleRenderTest::CleanUp()
//{
//    glDeleteVertexArrays(1, &VAO);
//    glDeleteBuffers(1, &VBO);
//	glDeleteTextures(1, &textureId);
//    return true;
//}