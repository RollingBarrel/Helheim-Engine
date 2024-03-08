#pragma once
#include "Resource.h"

#include <vector>
#include <string>

#include "float3.h"
#include "Quat.h"


struct ModelNode
{
	//Game Object
	std::string mName;

	math::float3 mTranslation;
	math::Quat mRotation;
	math::float3 mScale;

	std::vector<std::pair<unsigned int, unsigned int>> mUids;

	std::vector<ModelNode> mChildren;

	//Components
	int mMeshId;
	int mCameraId;
	int mSkinId;
};

class ResourceModel : public Resource
{
public:

	ResourceModel(unsigned int uid, ModelNode root);

	const ModelNode& GetRoot() const { return mRoot; }

private:

	ModelNode mRoot;
};

