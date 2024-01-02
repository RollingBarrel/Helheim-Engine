#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"

struct ResourceMesh
{
	unsigned int mNumVertices;
	unsigned int mNumIndices;

	const unsigned int* mIndices;
	const float* mVerticesPosition;
	const float* mVerticesTextureCoordinate;
	const float* mVerticesColor;
	const float* mVerticesNormal;
	const float* mVerticesTanget;
};

namespace Importer
{
	namespace Mesh
	{
		void Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, ResourceMesh& mesh);

		void Save(const ResourceMesh& mesh);

		void Load();
	}
}
