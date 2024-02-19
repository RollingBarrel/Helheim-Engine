#include "ResourceModel.h"

#include "ModuleFileSystem.h"

ResourceModel::ResourceModel(unsigned int uid, const char* path) : Resource(uid)
{
	mType = Type::Model;
	mAssetsFile = path;
	mLibraryFile = LIBRARY_MODEL_PATH + std::to_string(mUID) + ".model";
}

void ResourceModel::SetUids(unsigned int meshUID, unsigned int materialUID)
{
	mUids.push_back({ meshUID, materialUID });
}
