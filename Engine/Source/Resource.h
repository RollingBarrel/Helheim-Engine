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

	Resource(unsigned int uid, Type type);
	virtual ~Resource();
	Type GetType() const { return mType; };
	unsigned int GetUID() const;
	const char* GetAssetsFile() const;
	const char* GetLibraryFile() const;
	unsigned int GetReferenceCount() const;

	const void SetAssetsFile(const char* assetsFile) { mAssetsFile = assetsFile; }
	const void SetLibraryFile(const char* libraryFile) { mLibraryFile = libraryFile; }

protected:
	//Name??? To set it when generating game object
	unsigned int mUID = 0;
	std::string mAssetsFile;
	std::string mLibraryFile;
	Type mType = Type::Unknown;
	unsigned int mReferenceCount = 0;
};

