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
		Model,
		Material,
		Scene,
		Unknown
	};

	Resource(unsigned int uid);
	virtual ~Resource();

	Type GetType() const { return mType; };
	unsigned int GetUID() const { return mUID; };
	const std::string GetAssetsFile() const { return mAssetsFile; };
	const std::string GetLibraryFile() const { return mLibraryFile; };
	unsigned int GetReferenceCount() const { return mReferenceCount; };

	void SetAssetsFile(const std::string& assetsFile) { mAssetsFile = assetsFile; };
	void SetLibraryFile(const std::string& libraryFile) { mLibraryFile = libraryFile; };

protected:
	//Name??? To set it when generating game object
	unsigned int mUID = 0;
	std::string mAssetsFile;
	std::string mLibraryFile;
	Type mType = Type::Unknown;
	unsigned int mReferenceCount = 0;
};

