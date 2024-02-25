#include "ResourceModel.h"

#include "ModuleFileSystem.h"

ResourceModel::ResourceModel(unsigned int uid) : Resource(uid, Type::Model)
{
}

void ResourceModel::SetUids(unsigned int meshUID, unsigned int materialUID)
{
	mUids.push_back({ meshUID, materialUID });
}
