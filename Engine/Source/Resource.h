#pragma once
#include "Globals.h"
#include <string>

class ENGINE_API Resource
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
		Script = 9,
		Object = 10,
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
	unsigned int mUID;
	Type mType;
	unsigned int mReferenceCount;
};

