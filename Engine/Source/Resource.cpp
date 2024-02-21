#include "Resource.h"
#include "ModuleFileSystem.h"

#define _CRT_SECURE_NO_WARNINGS

Resource::Resource(unsigned int uid, Type type, const char* assetsPath, const char* libraryExtension) : mUID(uid), mType(type), mReferenceCount(0)
{
	char* tmp = const_cast<char*>(mLibraryFile);
	std::string path = LIBRARY_MESH_PATH + std::to_string(uid) + libraryExtension;
	tmp = new char[path.size() + 1];
	tmp[path.size()] = '\0';
	strcpy(tmp, path.c_str());
	tmp = const_cast<char*>(mAssetsFile);
	unsigned int size = strlen(assetsPath);
	tmp = new char[size + 1];
	tmp[size] = '\0';
	strcpy(tmp, assetsPath);
}

Resource::~Resource()
{
}

unsigned int Resource::AddReferenceCount() 
{
	++mReferenceCount;
	if (mReferenceCount == 1)
		LoadToMemory();
	return mReferenceCount; 
}
unsigned int Resource::RemoveReferenceCount() 
{
	--mReferenceCount;
	if (mReferenceCount == 0)
		UnloadFromMemory();
	return mReferenceCount; 
}