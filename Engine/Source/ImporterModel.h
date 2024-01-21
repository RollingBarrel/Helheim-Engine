#pragma once

struct ResourceModel
{
	unsigned int meshUID;
	unsigned int materiaUID;

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

