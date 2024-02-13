#pragma once
#include "Resource.h"

#include <vector>

#include "ImporterModel.h"

class ResourceModel : public Resource
{
public: 
	ResourceModel(unsigned int uid);

	typedef struct ModelIds {
		unsigned int meshUID;
		unsigned int materialUID;
	}ModelIds;

	const std::vector<ModelIds>& GetUids() const { return mUids; }
	void SetUids(unsigned int meshUID, unsigned int materialUID);

private:

	std::vector<ModelIds>mUids;
};

