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

	Resource(unsigned int uid, Type type);
	virtual ~Resource();
	Type GetType() const { return mType; };
	unsigned int GetUID() const;
	const char* GetAssetsFile() const;
	const char* GetLibraryFile() const;
	unsigned int GetReferenceCount() const;


protected:
	unsigned int mUID = 0;
	std::string mAssetsFile;
	std::string mLibraryFile;
	Type mType = Type::Unknown;
	unsigned int mReferenceCount = 0;
};

