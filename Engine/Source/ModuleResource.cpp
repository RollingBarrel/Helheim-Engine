#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "Resource.h"
#include "ResourceTexture.h"
#include "ResourceMesh.h"
#include "ResourceMaterial.h"
#include "ResourceAnimation.h"
#include "ResourceModel.h"
#include "ResourceScene.h"
#include "ResourcePrefab.h"
#include "ResourceScript.h"
#include "ResourceNavMesh.h"
#include "ResourceStateMachine.h"
#include "ResourceIBL.h"

#include "SaveLoadTexture.h"
#include "SaveLoadMesh.h"
#include "SaveLoadMaterial.h"
#include "SaveLoadAnimation.h"
#include "SaveLoadModel.h"
#include "SaveLoadScene.h"
#include "SaveLoadPrefab.h"
#include "SaveLoadScript.h"
#include "SaveLoadNavMesh.h"
#include "SaveLoadStateMachine.h"
#include "SaveLoadIBL.h"

#include "Archive.h"


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
	//App->GetFileSystem()->NormalizePath(path.data());

	char* fileBuffer = nullptr;
	if (!App->GetFileSystem()->Load(path.c_str(), &fileBuffer))
	{
		LOG("Not able to open .emeta file");
		return nullptr;
	}

	unsigned int uid = 0;
	Resource::Type type = Resource::Type::Unknown;

	Archive doc(fileBuffer);
	delete[] fileBuffer;
	JsonObject root = doc.GetRootObject();
	uid = root.GetInt("uid");
	type = static_cast<Resource::Type>(root.GetInt("type"));

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
	case Resource::Type::Animation:
	{
		ret = Importer::Animation::Load(lPath, uid);
		break;
	}
	case Resource::Type::Model:
	{
		ret = Importer::Model::Load(lPath, uid);
		break;
	}
	case Resource::Type::Scene:
	{
		ret = Importer::Scene::Load(lPath, uid);
		break;
	}
	case Resource::Type::Prefab:
	{
		ret = Importer::Prefab::Load(lPath, uid);
		break;
	}
	case Resource::Type::Script:
	{
		ret = Importer::Script::Load(lPath, uid);
		break;
	}
	case Resource::Type::NavMesh:
	{
		ret = Importer::NavMesh::Load(lPath, uid);
		break;
	}
	case Resource::Type::StateMachine:
	{
		ret = Importer::StateMachine::Load(lPath, uid);
		break;
	}
	case Resource::Type::IBL:
	{
		ret = Importer::IBL::Load(lPath, uid);
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
		if (mResources[uid]->GetReferenceCount() == 0)
		{
			delete mResources[uid];
			mResources.erase(uid);
		}
	}
}