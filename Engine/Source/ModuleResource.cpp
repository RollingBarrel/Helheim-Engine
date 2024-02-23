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
#include "ImporterMaterial.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <ctime>

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

	// Create copy in assets folder
	std::string assetsCopiedFile = DuplicateFileInAssetDir(importedFilePath, type);
	if (assetsCopiedFile.empty())
	{
		LOG("File could not be duplicated: %s", importedFilePath);
		return 0;
	}
	LOG("Succesfully duplicated the file in %s", assetsCopiedFile.c_str());

	//// Create the new resource
	Resource* resource = CreateNewResource(importedFilePath, assetsCopiedFile.c_str(), type); //Save ID, assetsFile path, libraryFile path, and create spesific resource
	if (resource == nullptr) 
	{
		LOG("Unable to create a new resource with this file: %s", importedFilePath);
		return 0;
	}

	// Create the Meta file for the Asset
	if (!CreateAssetsMeta(*resource))
	{
		LOG("Couldn't create a .meta File");
		return 0;
	}
	LOG("Succesfully created a .meta File");

	unsigned int ret = resource->GetUID();

	ReleaseResource(ret);

	return ret;
}

Resource* ModuleResource::RequestResource(const char* assetsPath)
{
	std::string path = assetsPath;
	path += ".meta";

	char* fileBuffer = nullptr;
	if (!App->GetFileSystem()->Load(path.c_str(), &fileBuffer))
	{
		LOG("Not able to open .meta file");
		return nullptr;
	}

	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse(fileBuffer);
	if (!result) {
		// Handle parsing error
		LOG("Not able to load .meta file");
		RELEASE_ARRAY(fileBuffer);
		return nullptr;
	}

	unsigned int uid = 0;
	Resource::Type type = Resource::Type::Unknown;
	if (document.HasMember("uid"))
	{
		uid = document["uid"].GetInt();
	}
	if (document.HasMember("type"))
	{
		type = static_cast<Resource::Type>(document["type"].GetInt());
	}
	
	RELEASE_ARRAY(fileBuffer);
	return RequestResource(uid, type);
}

Resource* ModuleResource::RequestResource(unsigned int uid, Resource::Type type)
{
	//Find if the resource is already loaded
	std::map<unsigned int, Resource*>::iterator it = mResources.find(uid);
	if (it != mResources.end())
	{
		it->second->AddReferenceCount();
		return it->second;
	}
	Resource* ret = nullptr;
	std::string path;
	switch (type)
	{
	case Resource::Type::Texture:
	{
		path += LIBRARY_TEXTURE_PATH;
		path += std::to_string(uid);
		path += LIBRARY_TEXTURE_EXTENSION;
		ret = Importer::Texture::Load(path.c_str(), uid);
		break;
	}
	case Resource::Type::Mesh:
	{
		path += LIBRARY_MESH_PATH;
		path += std::to_string(uid);
		path += LIBRARY_MESH_EXTENSION;
		ret = Importer::Mesh::Load(path.c_str(), uid);
		break;
	}
	case Resource::Type::Bone:
		break;
	case Resource::Type::Animation:
		break;
	case Resource::Type::Material:
	{
		path += LIBRARY_MATERIAL_PATH;
		path += std::to_string(uid);
		path += LIBRARY_MATERIAL_EXTENSION;
		ret = Importer::Material::Load(path.c_str(), uid);
		break;
	}
	case Resource::Type::Model:
	{
		path += LIBRARY_MODEL_PATH;
		path += std::to_string(uid);
		path += LIBRARY_MODEL_EXTENSION;
		ret = Importer::Model::Load(path.c_str(), uid);
		break;
	}
	case Resource::Type::Scene:
		break;
	case Resource::Type::NavMesh:
		break;
	case Resource::Type::Unknown:
		break;
	default:
		break;
	}
	//Buscar el resource amb luid a library
	//App->GetFileSystem()->FindFile(std::to_string(uid).c_str(), "Library");
	return ret;
}

Resource* ModuleResource::TryToLoadResource(const unsigned int uid, Resource::Type type)
{
	// This has a hardcode on the file extension if u have a better way to do it please do it, i'm useless
	Resource* ret;
	std::string libraryFilePath;
	switch (type) {
	case Resource::Type::Texture: 
		libraryFilePath = LIBRARY_TEXTURE_PATH + std::to_string(uid) + ".tex";
		ret = Importer::Texture::Load(libraryFilePath.c_str(), uid);
	default:
		LOG("Can't Load a resource with UID %i", uid);
		return nullptr;
	}
	//App->GetFileSystem()->GetPathFromFileName(uid);
	return ret;
}

void ModuleResource::ReleaseResource(unsigned int uid)
{
	if (mResources.find(uid) != mResources.end())
	{
		mResources[uid]->RemoveReferenceCount();
		if (mResources[uid]->GetReferenceCount() <= 0)
		{
			if (mResources.find(uid) == mResources.end())
			{
				LOG("The resource with UID %s does not exist.", std::to_string(uid).c_str());
			}
			else if (mResources.find(uid)->second != nullptr)
			{
				mResources[uid] = nullptr;
			}
			delete mResources[uid];
		}
	}
}

Resource* ModuleResource::CreateNewResource(const char* importedFile, const char* assetsFile, Resource::Type type)
{
	Resource* ret = nullptr;
	unsigned int uid = LCG().Int();

	switch (type)
	{
	case Resource::Type::Texture:
		ret = Importer::Texture::Import(assetsFile, uid);
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
		ret = Importer::Model::Import(importedFile, assetsFile, uid);
		if (ret) Importer::Model::Save((ResourceModel*)ret);
		break;
	case Resource::Type::Scene:
		break;
	case Resource::Type::NavMesh:
		break;
	default:
		LOG("Unable to Import, this file %s", assetsFile);
		break;
	}
	//if ret is not nullptr
	if (ret) {
		mResources[uid] = ret;
	}

	return ret;
}



std::string ModuleResource::DuplicateFileInAssetDir(const char* importedFilePath, const Resource::Type type) const
{
	std::string assetName;
	std::string extensionName;
	App->GetFileSystem()->SplitPath(importedFilePath, &assetName, &extensionName);
	std::string assetsFilePath;

	switch (type) {
	case Resource::Type::Texture:
	{
		assetsFilePath = ASSETS_TEXTURE_PATH + assetName + extensionName;
		break;
	}
	case Resource::Type::Model:
	{
		assetsFilePath = ASSETS_MODEL_PATH + assetName + extensionName;

		//Duplicate .bin file
		std::string importedBinFilePath = importedFilePath;
		unsigned int dotPos = importedBinFilePath.find_last_of('.');
		importedBinFilePath = importedBinFilePath.substr(0,dotPos);
		importedBinFilePath += ".bin";
		App->GetFileSystem()->CopyAbsolutePath(importedBinFilePath.c_str(), std::string(ASSETS_MODEL_PATH + assetName + ".bin").c_str());
		break;
	}
	default:
		LOG("Unable to Copy, this file %s", importedFilePath);
		break;
	}

	App->GetFileSystem()->CopyAbsolutePath(importedFilePath, assetsFilePath.c_str());

	return assetsFilePath;
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

	std::string metaName;
	metaName += resource.GetAssetsFile();
	metaName += ".meta";

	// Create a JSON document
	rapidjson::Document document;
	document.SetObject();

	// Add uid to the JSON document
	rapidjson::Value uidValue;
	uidValue.SetInt(resource.GetUID());
	document.AddMember("uid", uidValue, document.GetAllocator());

	// Add resource type to the JSON document
	rapidjson::Value typeValue;
	typeValue.SetInt(static_cast<int>(resource.GetType()));
	document.AddMember("type", typeValue, document.GetAllocator());

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
	ret = App->GetFileSystem()->Save(metaName.c_str(), buffer.GetString(), strlen(buffer.GetString()));

	buffer.Clear();

	return ret;
}