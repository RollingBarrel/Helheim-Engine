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
#ifdef ENGINE
	unsigned int ImportFile(const char* assetsFile, unsigned int forcedUid = 0, bool modifyAssets = true);
#endif // !ENGINE


	Resource* RequestResource(const char* assetsPath);
	Resource* RequestResource(unsigned int uid, Resource::Type type);
	void ReleaseResource(unsigned int uid);

private:
#ifdef ENGINE
	Resource* CreateNewResource(const char* assetsFile, const char* importedFile, Resource::Type type, unsigned int forcedUid = 0, bool modifyAssets = true);
	Resource::Type DeduceResourceType(const char* assetsFile);

	bool CreateAssetsMeta(const Resource& resource, const char* assetsFile) const;

	std::string DuplicateFileInAssetDir(const char* importedFilePath, const Resource::Type type) const;


	std::unordered_map<std::string, Resource::Type> mExtensionToResourceType = {
		{".png", Resource::Type::Texture},
		{".jpg", Resource::Type::Texture},
		{".bmp", Resource::Type::Texture},
		{".dds", Resource::Type::Texture},
		{".gltf", Resource::Type::Model},
		{".h", Resource::Type::Script},
		{".prfb", Resource::Type::Object},
		// Add more mappings for other resource types as needed
	};

	friend class ResourcePanel;
#endif // !ENGINE
	std::map<unsigned int, Resource*> mResources;
};

#endif //_MODULE_RESOURCE_H_