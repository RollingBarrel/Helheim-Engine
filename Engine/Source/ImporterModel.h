#pragma once
#include <vector>
struct ResourceModel
{
	typedef struct ModelIds {
		unsigned int meshUID;
		unsigned int materiaUID;
	}ModelIds;
	std::vector<ModelIds>mUids;

	unsigned int mUID;
};

namespace Importer
{
	namespace Model
	{
		void Import(const char* filePath, ResourceModel* rModel);

		void Save(const ResourceModel* ourModel);

		void Load(ResourceModel* ourModel, const char* fileName);

	}
};

