#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceModel.h"

#include <algorithm>

#include "Algorithm/Random/LCG.h"

#include "Importer.h"
#include "ImporterTexture.h"
#include "ImporterMesh.h"
#include "ImporterModel.h"


bool ModuleResource::Init() {
	mExtensionToResourceType = {
		{".png", Resource::Type::Texture},
		{".jpg", Resource::Type::Texture},
		{".bmp", Resource::Type::Texture},
		// Add more mappings for other resource types as needed
	};

	//mOurExtensionToResourceType = {
	//	{".tex", Resource::Type::Texture},
	//	// Add more mappings for other resource types as needed
	//};

}

unsigned int ModuleResource::Find(const char* assetsFile) const
{
	for (const auto& pair : mResources)
	{
		std::string assetName = "";
		App->GetFileSystem()->SplitPath(assetsFile, &assetName);
		if (assetName == assetsFile)
		{
			return pair.second->GetUID();
		}
	}

	// Return 0 if no matching resource is found
	return 0;
}

unsigned int ModuleResource::ImportFile(const char* importedFilePath)
{
	// Extract the import file type of asset
	Resource::Type type = DeduceResourceType(importedFilePath);
	if (type == Resource::Type::Unknown) 
	{
		LOG("Unable to import this unsoported file: %s", importedFilePath);
		return 0;
	}

	// Create the new resource
	Resource* resource = CreateNewResource(importedFilePath, type); //Save ID, assetsFile path, libraryFile path	
	if (resource == nullptr) 
	{
		LOG("Unable to create a new resource with this file: %s", importedFilePath);
		return 0;
	}

	// Create copy in assets folder
	if (!DuplicateFileInAssetDir(importedFilePath, *resource))
	{
		LOG("File could not be duplicated: %s", importedFilePath);
		return 0;
	}
	LOG("Succesfully duplicated the file in %s", resource->GetAssetsFile().c_str());

	// Create the Meta file for the Asset
	if (!CreateAssetsMeta(*resource))
	{
		LOG("Couldn't create a .meta File");
		return 0;
	}
	LOG("Succesfully created a .meta File");

	//Only Textures, Models, Scenes, Prefabs
	resource->Import(importedFilePath);
	resource->Save();
	/*switch (resource->GetType())
	{
		case Resource::Type::Texture:
		{
			(ResourceTexture*)resource->Import(importedFilePath);
			(ResourceTexture*)resource->Save();
			break;
		}
		case Resource::Type::Mesh:
		{
			//(ResourceMesh*)resource->Import(importedFilePath);
			//(ResourceMesh*)resource->Save();
			break;
		}
		case Resource::Type::Bone:
		{
			//Importer::Bone::Import(importedFilePath, (ResourceModel*)resource); 
			//Importer::Bone::Save((ResourceBone*)resource);
			break;
		}
		case Resource::Type::Animation:
		{
			//Importer::Animation::Import(importedFilePath, (ResourceModel*)resource);
			//Importer::Animation::Save((ResourceAnimation*)resource);
			break;
		}
		case Resource::Type::Material:
		{
			//Importer::Material::Import(importedFilePath, (ResourceModel*)resource);
			//Importer::Material::Save((ResourceMaterial*)resource);
			break;
		}
		case Resource::Type::Model:
		{
			Importer::Model::Import(importedFilePath, (ResourceModel*)resource);
			Importer::Model::Save((ResourceModel*)resource);
			break;
		}
		case Resource::Type::Scene:
		{
			//Importer::Scene::Import(importedFilePath, (ResourceModel*)resource);
			//Importer::Scene::Save((ResourceScene*)resource);
			break;
		}
		case Resource::Type::NavMesh:
		{
			//Importer::NavMesh::Import(importedFilePath, (ResourceModel*)resource);
			//Importer::NavMesh::Save((ResourceNavMesh*)resource);
			break;
		}
		default:
		{
			LOG("Unable to Import, this file %s", importedFilePath); 
			break;
		}
	}*/

	unsigned int ret = resource->GetUID();

	delete resource; // Unload the resource after importing, we should only use the ID

	return ret;
}

Resource* ModuleResource::RequestResource(unsigned int uid)
{
	//Find if the resource is already loaded
	std::map<unsigned int, Resource*>::iterator it = mResources.find(uid);
	if(it != mResources.end())
	{
		//it->second->referenceCount++;
		it->second->AddReferenceCount();
		return it->second;
	}
	else 
	{
		return TryToLoadResource(uid);
	}
	//Find the library file (if exists) and load the custom file format 
	//return TryToLoadResource(uid); <------------------------------------------TODO
	return nullptr;
}

Resource* ModuleResource::TryToLoadResource(const unsigned int uid)
{
	//App->GetFileSystem()->GetPathFromFileName();

	return nullptr;
}

void ModuleResource::ReleaseResource(Resource* resource)
{
	if (resource->GetReferenceCount() > 1)
	{
		resource->RemoveReferenceCount();
	}
	else 
	{
		if (resource->GetReferenceCount() == 1) resource->RemoveReferenceCount();
		
		mResources.erase(mResources.find(resource->GetUID())->first);
		delete resource;
	}
}

Resource* ModuleResource::CreateNewResource(const char* assetsFile, Resource::Type type)
{
	Resource* ret = nullptr;
	unsigned int uid = LCG().Int();

	std::string assetName;
	std::string extensionName;
	App->GetFileSystem()->SplitPath(assetsFile, &assetName, &extensionName);
	
	// Create resource from resource type
	std::string path;
	switch (type)
	{
	case Resource::Type::Texture: ret = new ResourceTexture(uid); path = ASSETS_TEXTURE_PATH; break;
	//case Resource::Type::Mesh: ret = new ResourceMesh(uid); path = ASSETS_MESH_PATH; break;
	//case Resource::Type::Bone: ret = new ResourceBone(uid); path = ASSETS_BONE_PATH; break;
	//case Resource::Type::Animation: ret = new ResourceAnimation(uid); path = ASSETS_ANIMATION_PATH; break;
	//case Resource::Type::Material: ret = new ResourceMaterial(uid); path = ASSETS_MATERIAL_PATH; break;
	//case Resource::Type::Model: ret = new ResourceModel(uid); path = ASSETS_MODEL_PATH; break; 
	//case Resource::Type::Scene: ret = new ResourceScene(uid); path = ASSETS_SCENES_PATH; break; 
	//case Resource::Type::NavMesh: ret = new ResourceNavMesh(uid); path = ASSETS_NAVMESH_PATH; break;
	default: LOG("Unable to create asset for this file: %s", assetsFile); break;
	}

	// if ret is not nullptr
	if (ret) {
		mResources[uid] = ret;
		ret->SetAssetsFile(path + assetName + extensionName);
		ret->SetLibraryFile(path + std::to_string(ret->GetUID()) + ret->GetExtension());
	}

	return ret;
}

const bool ModuleResource::DuplicateFileInAssetDir(const char* importedFilePath, const Resource& resource) const
{
	bool ret = true;
	
	/* FOR NOW ALL TYPES HAVE THE SAME CALL
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
	}*/

	ret = App->GetFileSystem()->CopyAbsolutePath(importedFilePath, resource.GetAssetsFile().c_str());

	return ret;
}

Resource::Type ModuleResource::DeduceResourceType(const char* assetsFile)
{
	// Extract file extension
	if (const char* fileExtension = strrchr(assetsFile, '.')) 
	{
		// Convert to lowercase
		std::string extension = fileExtension;
		std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower); 
		// Returns the Type of Resource (see map above)
		auto it = mExtensionToResourceType.find(extension);
		if (it != mExtensionToResourceType.end())
			return it->second;
	}
	// If the file extension is not recognized
	return Resource::Type::Unknown; 
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