#pragma once
#include "Resource.h"

#include <vector>

#include "float3.h"
#include "Quat.h"

typedef struct ModelIds {
	unsigned int meshUID;
	unsigned int materialUID;
}ModelIds;


struct ModelNode
{
	//Game Object
	const char* mName;

	math::float3 mTranslation;
	math::Quat mRotation;
	math::float3 mScale;

	int mMeshId;

	std::vector<ModelIds> mUids;
	std::vector<ModelNode> mChildren;

	//Components
	int mCameraId;
	int mSkinId;

};

class ResourceModel : public Resource
{
public:
	ResourceModel(unsigned int uid, ModelNode root);

private:

	ModelNode mRoot;
};

