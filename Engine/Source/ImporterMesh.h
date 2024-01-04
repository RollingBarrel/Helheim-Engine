#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"


struct ResourceMesh
{
	unsigned int mNumVertices;
	unsigned int mNumIndices;

	unsigned char* mIndices = nullptr;
	unsigned char* mVerticesPosition = nullptr;
	unsigned char* mVerticesTextureCoordinate = nullptr;
	unsigned char* mVerticesColor = nullptr;
	unsigned char* mVerticesNormal = nullptr;
	unsigned char* mVerticesTangent = nullptr;
};

namespace Importer
{
	namespace Mesh
	{
		void Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh& mesh);

		void Load(char** data, ResourceMesh& mesh);

		void Save(const ResourceMesh& mesh);
	}
}
