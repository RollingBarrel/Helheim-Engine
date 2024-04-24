#pragma once
#include "Resource.h"

#include <vector>
#include <string>

#include "float3.h"
#include "float4x4.h"
#include "Quat.h"


struct ModelNode
{
	//Game Object
	std::string mName;

	math::float3 mTranslation;
	math::Quat mRotation;
	math::float3 mScale;

	std::vector<std::pair<unsigned int, unsigned int>> mUids;

	int mParentIndex = -1;

	bool mHasTransform = false;
	//Components
	int mMeshId = -1;
	int mCameraId = -1;
	int mSkinId = -1;
};

class ResourceModel : public Resource
{
public:

	ResourceModel(unsigned int uid);

	const std::vector<ModelNode>& GetNodes() { return modelNodes; }

	std::vector<unsigned int> mAnimationUids;

	std::vector<ModelNode> modelNodes;
};