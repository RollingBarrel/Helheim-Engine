#pragma once

#include <string>

class Resource
{	
public:
	enum class Type
	{
		Texture,
		Mesh,
		Bone,
		Animation,
		Material,
		Model,
		Scene,
		NavMesh,
		Unknown
	};


	Resource();
	Resource(unsigned int uid);
	virtual ~Resource();

	virtual std::string GetExtension() = 0;

	Type GetType() const { return mType; };
	unsigned int GetUID() const { return mUID; };
	const std::string GetAssetsFile() const { return mAssetsFile; };
	const std::string GetLibraryFile() const { return mLibraryFile; };
	unsigned int GetReferenceCount() const { return mReferenceCount; };

	void SetAssetsFile(const std::string& assetsFile) { mAssetsFile = assetsFile; };
	void SetLibraryFile(const std::string& libraryFile) { mLibraryFile = libraryFile; };
	void AddReferenceCount() { mReferenceCount++; }
	void RemoveReferenceCount() { mReferenceCount--; }

protected:
	//Name??? To set it when generating game object
	unsigned int mUID;
	std::string mAssetsFile;
	std::string mLibraryFile;
	Type mType;
	unsigned int mReferenceCount;
};

