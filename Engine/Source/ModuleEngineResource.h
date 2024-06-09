#ifndef _MODULE_ENGINE_RESOURCE_H_
#define _MODULE_ENGINE_RESOURCE_H_

#include "ModuleResource.h"
#include <string>

class ModuleEngineResource : public ModuleResource
{
public:
	ModuleEngineResource() : ModuleResource() {}
	~ModuleEngineResource() {}
	bool Init() override;
	unsigned int ImportFile(const char* assetsFile, unsigned int forcedUid = 0, bool modifyAssets = true);
	bool CreateAssetsMeta(const Resource& resource, const char* assetsFile) const;
	Resource* CreateNewResource(const char* assetsFile, const char* importedFile, Resource::Type type, unsigned int forcedUid = 0, bool modifyAssets = true);
private:
	
	Resource::Type DeduceResourceType(const char* assetsFile);



	std::string DuplicateFileInAssetDir(const char* importedFilePath, const Resource::Type type) const;


	std::unordered_map<std::string, Resource::Type> mExtensionToResourceType = {
		{".png", Resource::Type::Texture},
		{".jpg", Resource::Type::Texture},
		{".bmp", Resource::Type::Texture},
		{".dds", Resource::Type::Texture},
		{".gltf", Resource::Type::Model},
		{".h", Resource::Type::Script},
		{".prfb", Resource::Type::Prefab},
		{".navmesshi", Resource::Type::NavMesh},
		{".smbin", Resource::Type::StateMachine}

		// Add more mappings for other resource types as needed
	};

	friend class ResourcePanel;
	std::map<unsigned int, Resource*> mResources;
};

#endif //_MODULE_ENGINE_RESOURCE_H_