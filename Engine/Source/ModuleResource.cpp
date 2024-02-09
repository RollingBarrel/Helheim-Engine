#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceModel.h"

#include <unordered_map>
#include <algorithm>

#include "Algorithm/Random/LCG.h"

#include "Importer.h"
#include "ImporterTexture.h"
#include "ImporterMesh.h"
#include "ImporterModel.h"

// Define a mapping between file extensions and resource types
const std::unordered_map<std::string, Resource::Type> extensionToResourceType = {
	{".png", Resource::Type::Texture},
	{".jpg", Resource::Type::Texture},
	{".bmp", Resource::Type::Texture},
	// Add more mappings for other resource types as needed
};


unsigned int ModuleResource::Find(const char* assetsFile) const
{
	return 0;
}

unsigned int ModuleResource::ImportFile(const char* importedFilePath)
{
	Resource::Type type = DeduceResourceType(importedFilePath);
	Resource* resource = CreateNewResource(importedFilePath, type); //Save ID, assetsFile path, libraryFile path	
	unsigned int ret = 0;

	//Create copy in assets folder
	if (DuplicateFileInAssetDir(importedFilePath, *resource))
	{
		LOG("Succesfully duplicated File in %s", resource->GetAssetsFile().c_str());
	}

	if (CreateAssetsMeta(*resource))
	{
		LOG("Succesfully Created a .meta File");
	}

	//Only Textures, Models, Scenes, Prefabs
	switch (resource->GetType())
	{
	case Resource::Type::Texture:
		Importer::Texture::Import(importedFilePath, (ResourceTexture*)resource);
		Importer::Texture::Save((ResourceTexture*)resource);
		break;
	case Resource::Type::Mesh:
		//Importer::Mesh::Import(importedFilePath, (ResourceMesh*) resource);
		//Importer::Mesh::Save((ResourceMesh*)resource);
		break;
	case Resource::Type::Bone:
		//Importer::Bone::Import(importedFilePath, (ResourceModel*)resource); 
		//Importer::Bone::Save((ResourceBone*)resource);
		break;
	case Resource::Type::Animation:
		//Importer::Animation::Import(importedFilePath, (ResourceModel*)resource);
		//Importer::Animation::Save((ResourceAnimation*)resource);
		break;
	case Resource::Type::Material:
		//Importer::Material::Import(importedFilePath, (ResourceModel*)resource);
		//Importer::Material::Save((ResourceMaterial*)resource);
		break;
	case Resource::Type::Model:
		Importer::Model::Import(importedFilePath, (ResourceModel*)resource);
		Importer::Model::Save((ResourceModel*)resource);
		break;
	case Resource::Type::Scene:
		//Importer::Scene::Import(importedFilePath, (ResourceModel*)resource);
		//Importer::Scene::Save((ResourceScene*)resource);
		break;
	case Resource::Type::NavMesh:
		//Importer::NavMesh::Import(importedFilePath, (ResourceModel*)resource);
		//Importer::NavMesh::Save((ResourceNavMesh*)resource);
		break;
	case Resource::Type::Unknown:
	default:
		LOG("Unable to Import, this file %s", importedFilePath); break;
	}

	ret = resource->GetUID();

	delete resource; //<-- unload the resource after importing, we should only use the ID

	return ret;
}


Resource* ModuleResource::RequestResource(unsigned int uid)
{
	return nullptr;
}

void ModuleResource::ReleaseResource(Resource* resource)
{
}


Resource* ModuleResource::CreateNewResource(const char* assetsFile, Resource::Type type)
{
	Resource* ret = nullptr;
	unsigned int uid = LCG().Int();

	std::string assetName;
	std::string extensionName;
	App->GetFileSystem()->SplitPath(assetsFile, &assetName, &extensionName);
	switch (type)
	{
		case Resource::Type::Texture:
		{
			ret = (Resource*) new ResourceTexture(uid); 
			mResources[uid] = ret;
			ret->SetAssetsFile(ASSETS_TEXTURE_PATH + assetName + extensionName);
			ret->SetLibraryFile(LIBRARY_TEXTURE_PATH + std::to_string(ret->GetUID()) + ".tex");
			break;
		}
		case Resource::Type::Mesh: ret = (Resource*) new ResourceMesh(uid); break;
		//case Resource::Type::Bone: ret = (Resource*) new ResourceBone(uid); break;
		//case Resource::Type::Animation: ret = (Resource*) new ResourceAnimation(uid); break;
		case Resource::Type::Material: ret = (Resource*) new ResourceMaterial(uid); break;
		case Resource::Type::Model: ret = (Resource*) new ResourceModel(uid); break;
	}

	return ret;
}

const bool ModuleResource::DuplicateFileInAssetDir(const char* importedFilePath, const Resource& resource) const
{
	bool ret = true;

	switch (resource.GetType())
	{
		case Resource::Type::Texture:
		{
			ret = App->GetFileSystem()->CopyAbsolutePath(importedFilePath, resource.GetAssetsFile().c_str());
			break;
		}
		case Resource::Type::Model:
		{
			ret = App->GetFileSystem()->CopyAbsolutePath(importedFilePath, resource.GetAssetsFile().c_str());
			break;
		}
		case Resource::Type::Unknown:
		{
			LOG("Unable to duplicate, this file %s", importedFilePath);
			ret = false;
			break;
		}
	}

	return ret;
}

Resource::Type ModuleResource::DeduceResourceType(const char* assetsFile)
{
	if (const char* fileExtension = strrchr(assetsFile, '.')) // Extract file extension
	{
		std::string extension = fileExtension;
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower); // Convert to lowercase

		auto it = extensionToResourceType.find(extension);
		if (it != extensionToResourceType.end())
			return it->second;
	}

	return Resource::Type::Unknown; // If the file extension is not recognized
}

const bool ModuleResource::CreateAssetsMeta(const Resource& resource) const
{
	bool ret = true;

	int metaFile = resource.GetAssetsFile().find_last_of('.');
	std::string metaName = resource.GetAssetsFile().substr(metaFile) + ".meta";

	char* buffer = new char[0];
	App->GetFileSystem()->Save(metaName.c_str(), buffer, sizeof(buffer));
	RELEASE_ARRAY(buffer);
	return ret;
}