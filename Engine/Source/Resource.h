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


	Resource(unsigned int uid, Type type);
	virtual ~Resource();

	Type GetType() const { return mType; }
	unsigned int GetUID() const { return mUID; }
	unsigned int GetReferenceCount() const { return mReferenceCount; }

	unsigned int AddReferenceCount() { return ++mReferenceCount; }
	unsigned int RemoveReferenceCount() { return --mReferenceCount; }

private:
	//Name??? To set it when generating game object
	unsigned int mUID;
	Type mType;
	unsigned int mReferenceCount;
};

