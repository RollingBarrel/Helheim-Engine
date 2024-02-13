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
		ResourceMesh* Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive);

		void Save(const ResourceMesh* ourMesh);

		void Load(ResourceMesh* ourMesh, const char* fileName);
	}
}