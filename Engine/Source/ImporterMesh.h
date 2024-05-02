#pragma once

class ResourceMesh;
namespace tinygltf {
	class Model;
	struct Primitive;
}

namespace Importer
{
	namespace Mesh
	{
		ResourceMesh* Import(const tinygltf::Model& model, const tinygltf::Primitive& primitive, unsigned int uid);
	}
}
