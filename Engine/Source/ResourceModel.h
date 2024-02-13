#pragma once
#include "Resource.h"

#include <vector>

#include "ImporterModel.h"

class ResourceModel : public Resource
{
	typedef struct ModelIds {
		unsigned int meshUID;
		unsigned int materialUID;
	}ModelIds;

public: 

	ResourceModel(unsigned int uid);

	const std::vector<ModelIds>& GetUids() const { return mUids; }
	void SetUids(unsigned int meshUID, unsigned int materialUID);

private:

	std::vector<ModelIds>mUids;
};

