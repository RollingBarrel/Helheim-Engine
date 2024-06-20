#pragma once
#include "Globals.h"

class ENGINE_API Resource
{
public:
	enum class Type
	{
		Texture = 1,
		Mesh = 2,
		Material = 3,
		Animation = 4,
		Model = 5,
		Scene = 6,
		Prefab = 7,
		Script = 8,
		NavMesh = 9,
		IBL = 10,
		StateMachine = 11,
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