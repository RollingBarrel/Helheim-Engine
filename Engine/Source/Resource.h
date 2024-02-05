#pragma once

#include <string>

class Resource
{
public:
	enum class Type
	{
		Texture,
		Mesh,
		Scene,
		Bone,
		Animation,
		Unknown
	};

	Resource(unsigned int uid);
	virtual ~Resource();

	virtual void Import(const char* filePath);
	virtual void Save();
	virtual unsigned int Load(const char* fileName = nullptr);

	Type GetType() const { return mType; };
	unsigned int GetUID() const { return mUID; };
	const std::string GetAssetsFile() const { return mAssetsFile; };
	const std::string GetLibraryFile() const { return mLibraryFile; };
	unsigned int GetReferenceCount() const { return mReferenceCount; };

	void SetAssetsFile(const std::string& assetsFile) { mAssetsFile = assetsFile; };
	void SetLibraryFile(const std::string& libraryFile) { mLibraryFile = libraryFile; };

protected:
	unsigned int mUID = 0;
	std::string mAssetsFile;
	std::string mLibraryFile;
	Type mType = Type::Unknown;
	unsigned int mReferenceCount = 0;
};

