#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "DirectXTex.h"

#include "Importer.h"
#include "ImporterModel.h"
#include "ImporterMesh.h"

#include "ModuleRenderTest.h"
#include "glew.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
#include "float4x4.h"
#include "MathConstants.h"
#include "imgui.h"

#include "GameObject.h"
#include "Component.h"
#include "MeshRendererComponent.h"
#include "ImporterTexture.h"
#include "ImporterMaterial.h"



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

static unsigned int SizeFromGlType(int glDefineType)
{
	switch (glDefineType)
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:
		return 1;
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_2_BYTES:
		return 2;
	case GL_3_BYTES:
		return 3;
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_FLOAT:
	case GL_4_BYTES:
		return 4;
	case GL_DOUBLE:
		return 8;
	default:
		LOG("WARNING: Could not identify GLTypeDefine");
	}
	return 0;
}

#include "mikktspace.h"
#include "weldmesh.h"

typedef struct {
	int numVertices;
	int posOffset;
	int texCoordOffset;
	int normOffset;
	int vertexSize;
	char* vertices;
	char* tVertices;
} MikkTSpaceStruct;

static int GetNumFaces(const SMikkTSpaceContext* pContext)
{
	MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
	return ptr->numVertices / 3;
}
static int GetNumVerticesOfFace(const SMikkTSpaceContext* pContext, const int iFace) {
	return 3;
}
static void GetPosition(const SMikkTSpaceContext* pContext, float fvPosOut[], const int iFace, const int iVert)
{
	MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
	float* posOut = (float*)&ptr->vertices[(iFace * 3 + iVert) * ptr->vertexSize + ptr->posOffset];
	fvPosOut[0] = posOut[0];
	fvPosOut[1] = posOut[1];
	fvPosOut[2] = posOut[2];
}
static void GetTexCoord(const SMikkTSpaceContext* pContext, float fvTexcOut[], const int iFace, const int iVert)
{
	MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
	float* texCOut = (float*)&ptr->vertices[(iFace * 3 + iVert) * ptr->vertexSize + ptr->texCoordOffset];
	fvTexcOut[0] = texCOut[0];
	fvTexcOut[1] = texCOut[1];
}
static void GetNormal(const SMikkTSpaceContext* pContext, float fvNormOut[], const int iFace, const int iVert)
{
	MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
	float* normalOut = (float*)&ptr->vertices[(iFace * 3 + iVert) * ptr->vertexSize + ptr->normOffset];
	fvNormOut[0] = normalOut[0];
	fvNormOut[1] = normalOut[1];
	fvNormOut[2] = normalOut[2];
}

static void SetTSpaceBasic(const SMikkTSpaceContext* pContext, const float fvTangent[], const float fSign, const int iFace, const int iVert)
{
	MikkTSpaceStruct* ptr = (MikkTSpaceStruct*)pContext->m_pUserData;
	//Escriure tota la info del vertex + les tangents
	const unsigned int vertexIdx = (iFace * 3 + iVert) * ptr->vertexSize;
	const unsigned int vertexTIdx = (iFace * 3 + iVert) * (ptr->vertexSize + sizeof(float) * 4);
	memcpy(&ptr->tVertices[vertexTIdx], &ptr->vertices[vertexIdx], ptr->vertexSize);
	memcpy(&ptr->tVertices[vertexTIdx + ptr->vertexSize], fvTangent, 3 * sizeof(float));
	memcpy(&ptr->tVertices[vertexTIdx + ptr->vertexSize + 3 * sizeof(float)], &fSign, sizeof(float));
}

void GenerateTangents(unsigned int indexType, unsigned int VBOEBO[1], unsigned int numIndices, unsigned int vertexSize)
{
	const unsigned int indexSize = SizeFromGlType(indexType);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOEBO[1]);
	const char* indices = (const char*)glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_READ_ONLY);
	glBindBuffer(GL_ARRAY_BUFFER, VBOEBO[0]);
	const char* vertices = (const char*)glMapBuffer(GL_ARRAY_BUFFER, GL_READ_ONLY);
	char* unweldedVertices = (char*)malloc(numIndices * vertexSize);
	for (int i = 0; i < numIndices; ++i)
	{
		unsigned long long vertexIdx = 0;
		memcpy(&vertexIdx, &indices[i * indexSize], indexSize);
		//TODO: possible error en la endianes
		//vertexIdx >>= sizeof(vertexIdx) - indexSize;
		memcpy(&unweldedVertices[i * vertexSize], &vertices[vertexIdx * vertexSize], vertexSize);
	}
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

	SMikkTSpaceInterface interfaceInput = {};
	interfaceInput.m_getNumFaces = GetNumFaces;
	interfaceInput.m_getNumVerticesOfFace = GetNumVerticesOfFace;
	interfaceInput.m_getNormal = GetNormal;
	interfaceInput.m_getPosition = GetPosition;
	interfaceInput.m_getTexCoord = GetTexCoord;
	interfaceInput.m_setTSpaceBasic = SetTSpaceBasic;
	MikkTSpaceStruct mikkInput = {};
	mikkInput.numVertices = numIndices;
	mikkInput.posOffset = 0;
	mikkInput.texCoordOffset = 3 * sizeof(float);
	mikkInput.normOffset = 5 * sizeof(float);
	mikkInput.vertexSize = 8 * sizeof(float);
	mikkInput.vertices = unweldedVertices;
	//Les mikktangents son vec4
	char* unweldedTVertices = (char*)malloc(numIndices * (vertexSize + 4 * sizeof(float)));
	mikkInput.tVertices = unweldedTVertices;
	SMikkTSpaceContext tangContext = {};
	tangContext.m_pInterface = &interfaceInput;
	tangContext.m_pUserData = &mikkInput;
	if (!genTangSpaceDefault(&tangContext))
		LOG("ERROR: Could not generate the tangent space");

	int* piRemapTable = (int*)malloc(mikkInput.numVertices * sizeof(int));
	float* pfVertexDataOut = (float*)malloc(mikkInput.numVertices * 12 * sizeof(float));
	unsigned int uniqueVertices = WeldMesh(piRemapTable, pfVertexDataOut,
		(float*)mikkInput.tVertices, mikkInput.numVertices, 12);
	free(unweldedTVertices);
	free(unweldedVertices);

	glBufferData(GL_ARRAY_BUFFER, uniqueVertices * 12 * sizeof(float), pfVertexDataOut, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(5 * sizeof(float)));
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(3);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mikkInput.numVertices * sizeof(int), piRemapTable, GL_STATIC_DRAW);
	free(piRemapTable);
	free(pfVertexDataOut);
}

bool ModuleRenderTest::Init()
{
	//Switch to Resource Shader later on.
	programId = CreateProgram("Assets/Shaders/PBR_VertexShader.glsl", "Assets/Shaders/PBR_PixelShader.glsl");

	Importer::Model::Import("Assets/Models/ZomBunny/Zombunny.gltf");

	//rMesh = new ResourceMesh();
	//rMesh2 = new ResourceMesh();
	//Importer::Mesh::Load(rMesh, "519408695");
	//Importer::Mesh::Load(rMesh2, "697368160");
	glUseProgram(programId);
	float4x4 model = float4x4::FromTRS(float3(1.0f, 0.0f, 0.0f), float4x4::RotateX(-pi / 4.0f), float3(2.5f, 2.5f, 2.5f));
	glUniformMatrix4fv(0, 1, GL_TRUE, model.ptr());


	glUniform3fv(1, 1, lightDir);
	glUniform3fv(2, 1, App->GetCamera()->GetPos().ptr());
	glUniform3fv(3, 1, lightCol);
	glUniform3fv(4, 1, ambientCol);
	glUniform1f(5, lightIntensity);


	textureId = LoadTexture("Assets\\Textures\\brickwall.jpg");
	normTextureId = LoadTexture("Assets\\Textures\\brickwall_normal.jpg");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glUniform1i(3, 0);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normTextureId);
	glUniform1i(9, 1);
    return true;
}
update_status ModuleRenderTest::Update()
{
	glUseProgram(programId);
	glUniform3fv(2, 1, App->GetCamera()->GetPos().ptr());
	ImGui::Begin("Lighting");
	if (ImGui::DragFloat("LightIntensity", &lightIntensity, 0.05f, 0.0f, 5.0f))
		glUniform1f(5, lightIntensity);
	if (ImGui::DragFloat3("LightDir", lightDir, 0.05f, -1.0f, 1.0f))
		glUniform3fv(1, 1, lightDir);
	if (ImGui::ColorPicker3("LightCol", lightCol))
		glUniform3fv(3, 1, lightCol);
	if (ImGui::ColorPicker3("AmbientCol", ambientCol))
		glUniform3fv(4, 1, ambientCol);
	ImGui::End();

	//App->GetOpenGL()->BindSceneFramebuffer();
	//glBindVertexArray(rMesh->GetVao());
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textureId);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, normTextureId);
	//glDrawElements(GL_TRIANGLES, rMesh->mNumIndices, GL_UNSIGNED_INT, 0);
	//
	//glBindVertexArray(rMesh2->GetVao());
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textureId);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, normTextureId);
	//glDrawElements(GL_TRIANGLES, rMesh2->mNumIndices, GL_UNSIGNED_INT, 0);

	glUseProgram(0);
	glBindVertexArray(0);
	App->GetOpenGL()->UnbindSceneFramebuffer();
    return UPDATE_CONTINUE;
}

bool ModuleRenderTest::CleanUp()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(2, VBOEBO);
	glDeleteTextures(1, &textureId);
	glDeleteTextures(1, &normTextureId);
    return true;
}