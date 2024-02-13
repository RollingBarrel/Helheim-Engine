#include "ResourceModel.h"

ResourceModel::ResourceModel(unsigned int uid) : Resource(uid)
{
}

void ResourceModel::SetUids(unsigned int meshUID, unsigned int materialUID)
{
	mUids.push_back({ meshUID, materialUID });
}
