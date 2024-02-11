#ifndef _MODULE_RESOURCE_H_
#define _MODULE_RESOURCE_H_

#include "Module.h"
#include "Resource.h"
#include <map>
#include <unordered_map>

class ModuleResource : public Module
{
public:

	bool Init() override;

	unsigned int Find(const char* assetsFile) const;
	unsigned int ImportFile(const char* assetsFile);
	unsigned int GenerateNewUID();

	Resource* RequestResource(unsigned int uid);
	void ReleaseResource(Resource* resource);

private:
	Resource* CreateNewResource(const char* assetsFile, Resource::Type type);
	Resource* TryToLoadResource(const unsigned int uid);
	Resource::Type DeduceResourceType(const char* assetsFile);

	const bool CreateAssetsMeta(const Resource& assetsFile) const;

	const bool DuplicateFileInAssetDir(const char* importedFilePath, const Resource& resource) const;

	std::map<unsigned int, Resource*> mResources;

	std::unordered_map<std::string, Resource::Type> mExtensionToResourceType;
	//std::unordered_map<std::string, Resource::Type> mOurExtensionToResourceType;
};

#endif //_MODULE_RESOURCE_H_