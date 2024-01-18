#pragma once
#include "Globals.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

class ResourceMaterial;

class Mesh
{
public:
	Mesh();
	~Mesh();

	void LoadVBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);
	void LoadEBO(const tinygltf::Model& model, const tinygltf::Mesh& mesh, const tinygltf::Primitive& primitive);
	void CreateVAO();

	void Render(float lightDir[3], float lightColor[3], float lightIntensity, float diffuseColor[3], float specularColor[3], float shininess, float ambientColor[3], int hasDiffuseMap, int hasSpecularMap, int hasShininessMap);

	int GetNumIndices() { return m_numIndices; }
	void SetMaterial(ResourceMaterial* material) { mMaterial = material; }


private:
	unsigned m_vbo;
	unsigned m_ebo;
	unsigned m_vao;

	int m_numVertices;
	int m_numIndices;

	ResourceMaterial* mMaterial = nullptr;
};