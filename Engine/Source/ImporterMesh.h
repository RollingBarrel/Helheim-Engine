#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"
#include <vector>


struct ResourceMesh;

namespace Importer
{
	namespace Mesh
	{
		ResourceMesh* Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, unsigned int uid);

		void Save(const ResourceMesh* ourMesh);

		ResourceMesh* Load(const char* fileName, unsigned int uid);
	}
}