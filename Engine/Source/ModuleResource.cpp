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

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <ctime>

bool ModuleResource::Init() {
	mExtensionToResourceType = {
		{".png", Resource::Type::Texture},
		{".jpg", Resource::Type::Texture},
		{".bmp", Resource::Type::Texture},
		// Add more mappings for other resource types as needed
	};

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

	//// Create the new resource
	Resource* resource = CreateNewResource(importedFilePath, type); //Save ID, assetsFile path, libraryFile path, and create spesific resource
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

	unsigned int ret = resource->GetUID();

	delete resource; // Unload the resource after importing, we should only use the ID

	return ret;
}

Resource* ModuleResource::RequestResource(unsigned int uid, Resource::Type type)
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
		return TryToLoadResource(uid, type);
	}
	//Find the library file (if exists) and load the custom file format 
	//return TryToLoadResource(uid); <------------------------------------------TODO
	return nullptr;
}

Resource* ModuleResource::TryToLoadResource(const unsigned int uid, Resource::Type type)
{
	//App->GetFileSystem()->GetPathFromFileName(uid);

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

	switch (type)
	{
	case Resource::Type::Texture:
		ret = Importer::Texture::Import(ret->GetAssetsFile().c_str(), uid);
		if (ret) Importer::Texture::Save((ResourceTexture*)ret);
		break;
	case Resource::Type::Mesh:
		break;
	case Resource::Type::Bone:
		break;
	case Resource::Type::Animation:
		break;
	case Resource::Type::Material:
		break;
	case Resource::Type::Model:
		//std::string binFile = "";
		//App->GetFileSystem()->SplitPath(importedFilePath, &binFile);

		//App->GetFileSystem()->CopyAbsolutePath(importedFilePath, std::string(ASSETS_MODEL_PATH + binFile + ".bin").c_str());

		//Importer::Model::Import(resource->GetAssetsFile().c_str(), (ResourceModel*)resource);
		//Importer::Model::Save((ResourceModel*)resource);
		break;
	case Resource::Type::Scene:
		break;
	case Resource::Type::NavMesh:
		break;
	default:
		LOG("Unable to Import, this file %s", ret->GetAssetsFile().c_str());
		break;
	}

	// if ret is not nullptr
	if (ret) {
		mResources[uid] = ret;
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

	// Get the path of the .meta file
	std::string assetName = "";
	std::string assetExtension = "";
	App->GetFileSystem()->SplitPath(resource.GetAssetsFile().c_str(), &assetName, &assetExtension);

	std::string metaName = assetName + assetExtension + ".meta";

	// Create a JSON document
	rapidjson::Document document;
	document.SetObject();

	// Add uid to the JSON document
	rapidjson::Value uidValue;
	uidValue.SetInt(resource.GetUID());
	document.AddMember("uid", uidValue, document.GetAllocator());

	// Add time of creation to the JSON document
	rapidjson::Value timeValue;
	time_t currentTime = time(nullptr);
	timeValue.SetInt(static_cast<int>(currentTime));
	document.AddMember("time", timeValue, document.GetAllocator());

	// Convert the JSON document to a string
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);
	const char* jsonStr = buffer.GetString();

	// Save the JSON string to the .meta file
	ret = App->GetFileSystem()->Save(metaName.c_str(), jsonStr, strlen(jsonStr));
	
	RELEASE_ARRAY(jsonStr);
	
	return ret;
}