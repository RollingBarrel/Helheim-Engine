#pragma once
#include "Importer.h"

struct Mesh
{
	unsigned mNumVertices;
	unsigned mNumIndices;
};

class ImporterMesh : public Importer
{
public:

	void Import(const char* filePath) const;
};
