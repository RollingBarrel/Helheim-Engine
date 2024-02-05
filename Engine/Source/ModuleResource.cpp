#include "Application.h"
#include "ModuleFileSystem.h"
#include "ModuleResource.h"
#include "Resource.h"
#include "ResourceTexture.h"

#include <unordered_map>
#include <algorithm>

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

unsigned int ModuleResource::ImportFile(const char* assetsFile)
{
	Resource::Type type = DeduceResourceType(assetsFile);
	Resource* resource = CreateNewResource(assetsFile, type); //Save ID, assetsFile path, libraryFile path	
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
	static unsigned int currentUID = 0; // Initialize a static variable to track the current UID
	return ++currentUID; // Increment and return the current UID
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
	switch (type)
	{
	case Resource::Type::Texture: ret = (Resource*) new ResourceTexture(uid); break;
	//case Resource::mesh: ret = (Resource*) new ResourceMesh(uid); break;
	//case Resource::scene: ret = (Resource*) new ResourceScene(uid); break;
	//case Resource::bone: ret = (Resource*) new ResourceBone(uid); break;
	//case Resource::animation: ret = (Resource*) new ResourceAnimation(uid); break;
	}
	if (ret != nullptr)
	{
		mResources[uid] = ret;
		ret->SetAssetsFile(assetsPath);
		ret->SetLibraryFile(GenLibraryPath(resource));
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