#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"

#include "Importer.h"
#include "ImporterTexture.h"

unsigned int ModuleResource::Find(const char* assetsFile) const
{
	return 0;
}

unsigned int ModuleResource::ImportFile(const char* assetsFile)
{

	Resource* resource = CreateNewResource(assetsFile, Resource::Type::Unknown); //Save ID, assetsFile path, libraryFile path	
	unsigned int ret = 0;

	//Create copy in assets folder

	char* fileBuffer;
	App->GetFileSystem()->Load(assetsFile, &fileBuffer); //<-- pseudocode, load from File System

	switch (resource->GetType())
	{
	//case Resource::Type::Texture: Importer::Texture::Import(fileBuffer, resource); break;
	//case Resource::scene: App->scene->Import(fileBuffer, resource); break;
	//case Resource::mesh: App->meshes->Import(fileBuffer, resource); break;
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
	return nullptr;
}
