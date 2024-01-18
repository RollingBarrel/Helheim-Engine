#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"


struct ResourceMesh;

namespace Importer
{
	namespace Mesh
	{
		void Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* ourMesh);

		void Save(const ResourceMesh* ourMesh);

		void Load(char* fileBuffer, ResourceMesh* ourMesh, const char* fileName);
	}
}

struct ResourceMesh
{
	unsigned int mNumVertices;
	unsigned int mNumIndices;

	unsigned int* mIndices = nullptr;
	float* mVerticesPosition = nullptr;
	float* mVerticesTextureCoordinate = nullptr;
	float* mVerticesNormal = nullptr;
	float* mVerticesTangent = nullptr;
	float* mVerticesColor = nullptr;


	const char* mMeshName = nullptr;
	unsigned int mUID;

	void FromInterleavedData(float*vData, unsigned int numVertices,  unsigned int* iData, unsigned int numIndices, char attributeMask);
	float* GetInterleavedData() const;
	unsigned int GetVertexSize() const { return mVertexSize; }

	void LoadVAO();
	void LoadVBO();
	void LoadEBO();

	unsigned int GetVAO() { return mVao; };
	unsigned int GetVBO() { return mVbo; };
	unsigned int GetEBO() { return mEbo; };

private:
	void LoadInterleavedAttribute(float* fillBuffer, const float* attribData, unsigned int& attribFloatsOffset, unsigned int attribElements, unsigned int elementFloats) const;

	unsigned int mVao;
	unsigned int mVbo;
	unsigned int mEbo;
	unsigned int mVertexSize;

	friend void Importer::Mesh::Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* mesh);
	friend void Importer::Mesh::Load(char* fileBuffer, ResourceMesh* mesh, const char* fileName);
};
