#pragma once
#include "Resource.h"

#include <vector>

#include "ImporterModel.h"

class ResourceModel : public Resource
{
public: 
	friend void Importer::Model::Save(const ResourceModel* ourModel);
	friend void Importer::Model::Load(ResourceModel* ourModel, const char* fileName);

	ResourceModel(unsigned int uid);

	typedef struct ModelIds {
		unsigned int meshUID;
		unsigned int materialUID;
	}ModelIds;

	std::vector<ModelIds>mUids;
};

