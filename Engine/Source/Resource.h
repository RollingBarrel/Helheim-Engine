#pragma once

#include <string>

class Resource
{	
public:
	enum class Type
	{
		Texture = 1,
		Mesh = 2,
		Bone = 3,
		Animation = 4,
		Material = 5,
		Model = 6,
		Scene = 7,
		NavMesh = 8,
		Unknown = 0
	};


	Resource(unsigned int uid, Type type, const char* assetsPath, const char* libraryExtension);
	virtual ~Resource();

	Type GetType() const { return mType; };
	unsigned int GetUID() const { return mUID; };
	const char* GetAssetsFile() const { return mAssetsFile; };
	const char* GetLibraryFile() const { return mLibraryFile; };
	unsigned int GetReferenceCount() const { return mReferenceCount; };

	unsigned int AddReferenceCount();
	unsigned int RemoveReferenceCount();
	virtual void LoadToMemory() {}
	virtual void UnloadFromMemory() {}

private:
	//Name??? To set it when generating game object
	unsigned int mUID;
	const char* mAssetsFile;
	const char* mLibraryFile;
	Type mType;
	unsigned int mReferenceCount;
};

