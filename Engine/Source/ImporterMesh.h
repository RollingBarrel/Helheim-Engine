#pragma once

struct ResourceMesh
{
	unsigned mNumVertices;
	unsigned mNumIndices;
};

namespace Importer
{
	namespace Mesh
	{
		void Import(const char* filePath);
	}
}
