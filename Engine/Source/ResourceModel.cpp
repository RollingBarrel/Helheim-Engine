#include "ResourceModel.h"

#include "ModuleFileSystem.h"

ResourceModel::ResourceModel(unsigned int uid, const char* path) : Resource(uid, Type::Model, path, ".model")
{
}

void ResourceModel::SetUids(unsigned int meshUID, unsigned int materialUID)
{
	mUids.push_back({ meshUID, materialUID });
}
