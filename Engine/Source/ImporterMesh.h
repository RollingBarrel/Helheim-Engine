#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"


struct ResourceMesh
{
	unsigned int mNumVertices;
	unsigned int mNumIndices;

	unsigned unsigned int* mIndices = nullptr;
	float* mVerticesPosition = nullptr;
	float* mVerticesTextureCoordinate = nullptr;
	float* mVerticesColor = nullptr;
	float* mVerticesNormal = nullptr;
	float* mVerticesTangent = nullptr;

	const char* mMeshName = nullptr;

	void LoadVAO();
	void LoadVBO();
	void LoadEBO();

	unsigned int GetVAO() { return mVao; };
	unsigned int GetVBO() { return mVbo; };
	unsigned int GetEBO() { return mEbo; };
	
private:
	unsigned int mVao;
	unsigned int mVbo;
	unsigned int mEbo;
};

namespace Importer
{
	namespace Mesh
	{
		void Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh* ourMesh);

		void Save(const ResourceMesh* ourMesh);

		void Load(char* fileBuffer, ResourceMesh* ourMesh, const char* fileName);
	}
}
