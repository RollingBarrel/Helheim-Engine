#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceModel.h"
#include "ResourceScript.h"
#include "ResourceAnimation.h"

#include "SaveLoadModel.h"
#include "SaveLoadMesh.h"
#include "SaveLoadMaterial.h"
#include "SaveLoadAnimation.h"
#include "SaveLoadScript.h"
#include "SaveLoadTexture.h"

#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"


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

Resource* ModuleResource::RequestResource(const char* assetsPath)
{
	std::string path = assetsPath;
	path += ".emeta";

	char* fileBuffer = nullptr;
	if (!App->GetFileSystem()->Load(path.c_str(), &fileBuffer))
	{
		LOG("Not able to open .emeta file");
		return nullptr;
	}

	rapidjson::Document document;
	rapidjson::ParseResult result = document.Parse(fileBuffer);
	if (!result) {
		// Handle parsing error
		LOG("Not able to load .emeta file");
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
	const char* lPath = App->GetFileSystem()->GetLibraryFile(uid);
	switch (type)
	{
	case Resource::Type::Texture:
	{
		ret = Importer::Texture::Load(lPath, uid);
		break;
	}
	case Resource::Type::Mesh:
	{
		ret = Importer::Mesh::Load(lPath, uid);
		break;
	}
	case Resource::Type::Material:
	{
		ret = Importer::Material::Load(lPath, uid);
		break;
	}
	case Resource::Type::Model:
	{
		ret = Importer::Model::Load(lPath, uid);
		break;
	}
	case Resource::Type::Script:
	{
		ret = Importer::Script::Load(lPath, uid);
		break;
	}
	case Resource::Type::Animation:
	{
		ret = Importer::Animation::Load(lPath, uid);
		break;
	}
	case Resource::Type::Object:
	{
		ret = new Resource(uid, type);
		break;
	}
	default:
		break;
	}
	delete[] lPath;
	if (ret)
	{
		ret->AddReferenceCount();
		mResources[uid] = ret;
	}
	return ret;
}

void ModuleResource::ReleaseResource(unsigned int uid)
{
	if (mResources.find(uid) != mResources.end())
	{
		mResources[uid]->RemoveReferenceCount();
		if (mResources[uid]->GetReferenceCount() <= 0)
		{
			delete mResources[uid];
			mResources.erase(uid);
		}
	}
}