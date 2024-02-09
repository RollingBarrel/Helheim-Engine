#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "Importer.h"
#include "ImporterTexture.h"
#include "ImporterMesh.h"
#include "ImporterModel.h"

unsigned int ModuleResource::Find(const char* assetsFile) const
{
	return 0;
}

unsigned int ModuleResource::ImportFile(const char* importedFilePath)
{
	Resource* resource = CreateNewResource(importedFilePath, Resource::Type::Unknown); //Save ID, assetsFile path, libraryFile path	
	unsigned int ret = 0;

	//Create copy in assets folder
	DuplicateFileInAssetDir(importedFilePath, *resource);

	//Only Textures, Models, Scenes, Prefabs
	switch (resource->GetType())
	{
	//case Resource::Type::Texture: Importer::Texture::Import(fileBuffer, resource); break;
	//case Resource::scene: App->scene->Import(fileBuffer, resource); break;
	//case Resource::Type::Mesh: Importer::Model::Import(fileBuffer, resource); break;
	}
	//SaveResource(resource);
	//ret = resource->GetID();
	//RELEASE_ARRAY(fileBuffer);
	//UnloadResource(resource); //<-- unload the resource after importing, we should only use the ID
	return ret;
}

unsigned int ModuleResource::GenerateNewUID()
{
	return 0;
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
	unsigned int uid = GenerateNewUID(); // Your own algorithm to generate new IDs. Random // MD5

	std::string assetsPath;

	switch (type)
	{
		//case Resource::Type::Texture: ret = (Resource*) new ResourceTexture(uid); break;
		//case Resource::mesh: ret = (Resource*) new ResourceMesh(uid); break;
		//case Resource::bone: ret = (Resource*) new ResourceBone(uid); break;
		//case Resource::animation: ret = (Resource*) new ResourceAnimation(uid); break;
		case Resource::Type::Model:
		{
			ret = (Resource*) new ResourceModel(uid);
			ret.SetName();
			assetsPath = ASSETS_PATH + ;
			break;
		}
		//case Resource::scene: ret = (Resource*) new ResourceScene(uid); break;
	}
	if (ret != nullptr)
	{
		mResources[uid] = ret;
		ret->SetAssetsFile(assetsPath.c_str());
		ret->SetLibraryFile(GenLibraryPath(resource));
	}
	return ret;
}

const void ModuleResource::DuplicateFileInAssetDir(const char* importedFilePath, const Resource& resource) const
{

	switch (resource.GetType())
	{
		case Resource::Type::Texture:
		{
			App->GetFileSystem()->CopyAbsolutePath(importedFilePath, resource.GetAssetsFile());
			break;
		}		
		case Resource::Type::Model:
		{
			App->GetFileSystem()->CopyAbsolutePath(importedFilePath, resource.GetAssetsFile());
			break;
		}
		case Resource::Type::Unknown:
		{
			LOG("Unable to duplicate, this file %s", importedFilePath)
			break;
		}
	}

}
