#include "ModuleEngineResource.h"
#include "EngineApp.h"
#include "ModuleFileSystem.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceModel.h"
#include "ResourceScript.h"
#include "ResourceAnimation.h"
#include "ResourceNavMesh.h"
#include "ResourceStateMachine.h"

#include <algorithm>

#include "Algorithm/Random/LCG.h"

#include "ImporterTexture.h"
#include "ImporterModel.h"
#include "ImporterScript.h"
#include "ImporterNavMesh.h"
#include "ImporterStateMachine.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

bool ModuleEngineResource::Init()
{
	std::vector<std::string> metas;
	EngineApp->GetFileSystem()->DiscoverFiles("Assets", ".emeta", metas);
	std::string assetsPath;
	for (std::string meta : metas)
	{
		assetsPath = meta.substr(0, meta.find_last_of('.'));
		if (!EngineApp->GetFileSystem()->Exists(assetsPath.c_str()))
		{
			EngineApp->GetFileSystem()->RemoveFile(meta.c_str());
			continue;
		}

		char* fileBuffer = nullptr;
		if (!EngineApp->GetFileSystem()->Load(meta.c_str(), &fileBuffer))
		{
			LOG("Not able to open .emeta file");
			continue;
		}

		rapidjson::Document document;
		rapidjson::ParseResult result = document.Parse(fileBuffer);
		if (!result)
		{
			// Handle parsing error
			LOG("Not able to load .emeta file");
			RELEASE_ARRAY(fileBuffer);
			continue;
		}

		unsigned int uid = 0;
		Resource::Type type = Resource::Type::Unknown;
		assert(document.HasMember("uid") && "Meta has no uid");
		uid = document["uid"].GetInt();
		int64_t metaAssetModTime;
		//Mod time
		assert(document.HasMember("modTime") && "Meta has no mod time");
		metaAssetModTime = document["modTime"].GetInt64();
		int64_t metaCreationTime = EngineApp->GetFileSystem()->GetCreationTime(meta.c_str());
		int64_t assetModTime = EngineApp->GetFileSystem()->GetLastModTime(assetsPath.c_str());

		//if the meta time is very different compared to the time it stores inside the date has probably been modified by a git clone
		if (metaCreationTime > (metaAssetModTime + 10))
		{

			int64_t  diff = metaCreationTime - metaAssetModTime;
			metaAssetModTime += diff;
		}
		const char* libraryFile = EngineApp->GetFileSystem()->GetLibraryFile(uid);
		if (metaAssetModTime < assetModTime)
		{
			ImportFile(assetsPath.c_str(), uid);
		}
		else if (!EngineApp->GetFileSystem()->Exists(libraryFile))
		{
			ImportFile(assetsPath.c_str(), uid, false);
		}
		delete[] libraryFile;

		if (fileBuffer) {
			delete[] fileBuffer;
		}
	}
	return true;
}

unsigned int ModuleEngineResource::ImportFile(const char* importedFilePath, unsigned int forcedUid, bool modifyAssets)
{
	// Extract the import file type of asset
	Resource::Type type = DeduceResourceType(importedFilePath);
	if (type == Resource::Type::Unknown)
	{
		LOG("Unable to import this unsoported file: %s", importedFilePath);
		return 0;
	}

	std::string assetsCopiedFile;
	// Create copy in assets folder
	if (modifyAssets)
	{
		assetsCopiedFile = DuplicateFileInAssetDir(importedFilePath, type);
		if (assetsCopiedFile.empty())
		{
			LOG("File could not be duplicated: %s", importedFilePath);
			return 0;
		}
		LOG("Succesfully duplicated the file in %s", assetsCopiedFile.c_str());
	}
	else
		assetsCopiedFile = importedFilePath;
	//// Create the new resource
	Resource* resource = CreateNewResource(assetsCopiedFile.c_str(), importedFilePath, type, forcedUid, modifyAssets); //Save ID, assetsFile path, libraryFile path, and create spesific resource
	if (resource == nullptr)
	{
		LOG("Unable to create a new resource with this file: %s", importedFilePath);
		return 0;
	}

	// Create the Meta file for the Asset
	if (modifyAssets)
	{
		if (!CreateAssetsMeta(*resource, assetsCopiedFile.c_str()))
		{
			LOG("Couldn't create a .emeta File");
			return 0;
		}
		LOG("Succesfully created a .emeta File");
	}

	unsigned int ret = resource->GetUID();
	ReleaseResource(ret);
	return ret;
}

Resource* ModuleEngineResource::CreateNewResource(const char* assetsFile, const char* importedFile, Resource::Type type, unsigned int forcedUid, bool modifyAssets)
{
	Resource* ret = nullptr;

	unsigned int uid;
	(forcedUid == 0) ? uid = LCG().Int() : uid = forcedUid;

	switch (type)
	{
	case Resource::Type::Texture:
		ret = Importer::Texture::Import(assetsFile, uid);
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
		ret = Importer::Model::Import(importedFile, uid, modifyAssets);
		break;
	case Resource::Type::Scene:
		break;
	case Resource::Type::NavMesh:
		ret = Importer::NavMesh::Import(uid, assetsFile);
		break;
	case Resource::Type::Script:
		ret = Importer::Script::Import(importedFile, uid);
		break;
	case Resource::Type::Object:
		ret = new Resource(uid, type);
		break;
	case Resource::Type::StateMachine:
		ret = Importer::StateMachine::Import(assetsFile, uid);
	default:
		LOG("Unable to Import, this file %s", assetsFile);
		break;
	}
	//if ret is not nullptr
	if (ret) {
		mResources[uid] = ret;
		mResources[uid]->AddReferenceCount();
	}

	return ret;
}



std::string ModuleEngineResource::DuplicateFileInAssetDir(const char* importedFilePath, const Resource::Type type) const
{
	std::string assetName;
	std::string extensionName;
	EngineApp->GetFileSystem()->SplitPath(importedFilePath, &assetName, &extensionName);
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
		importedBinFilePath = importedBinFilePath.substr(0, dotPos);
		importedBinFilePath += ".bin";
		EngineApp->GetFileSystem()->CopyAbsolutePath(importedBinFilePath.c_str(), std::string(ASSETS_MODEL_PATH + assetName + ".bin").c_str());
		break;
	}
	case Resource::Type::Object:
		assetsFilePath = importedFilePath;
		break;
	case Resource::Type::Script:
	{
		assetsFilePath = ASSETS_SCRIPT_PATH + assetName + extensionName;
		break;
	}
	default:
		LOG("Unable to Copy, this file %s", importedFilePath);
		break;
	}

	EngineApp->GetFileSystem()->CopyAbsolutePath(importedFilePath, assetsFilePath.c_str());

	return assetsFilePath;
}


Resource::Type ModuleEngineResource::DeduceResourceType(const char* assetsFile)
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

bool ModuleEngineResource::CreateAssetsMeta(const Resource& resource, const char* assetsFile) const
{
	bool ret = true;

	// Get the path of the .meta file

	std::string metaName;
	metaName += assetsFile;
	metaName += ".emeta";

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

	// Add modification time of asset to the JSON document
	rapidjson::Value timeValue;
	int64_t currentTime = EngineApp->GetFileSystem()->GetLastModTime(assetsFile);
	timeValue.SetInt64(currentTime);
	document.AddMember("modTime", timeValue, document.GetAllocator());

	// Convert the JSON document to a string
	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	document.Accept(writer);
	const char* jsonStr = buffer.GetString();

	EngineApp->GetFileSystem()->RemoveFile(metaName.c_str());
	// Save the JSON string to the .meta file
	ret = EngineApp->GetFileSystem()->Save(metaName.c_str(), buffer.GetString(), strlen(buffer.GetString()));

	buffer.Clear();

	return ret;
}