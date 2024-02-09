#ifndef _MODULE_RESOURCE_H_
#define _MODULE_RESOURCE_H_

#include "Module.h"
#include "Resource.h"
#include <unordered_map>

class ModuleResource : public Module
{
public:

	unsigned int Find(const char* assetsFile) const;
	unsigned int ImportFile(const char* assetsFile);
	unsigned int GenerateNewUID();

	Resource* RequestResource(unsigned int uid);
	void ReleaseResource(Resource* resource);

private:
	Resource* CreateNewResource(const char* assetsFile, Resource::Type type);
	Resource::Type DeduceResourceType(const char* assetsFile);

	const bool CreateAssetsMeta(const Resource& assetsFile) const;

	const bool DuplicateFileInAssetDir(const char* importedFilePath, const Resource& resource) const;

	std::unordered_map<unsigned int, Resource*> mResources;
};

#endif //_MODULE_RESOURCE_H_