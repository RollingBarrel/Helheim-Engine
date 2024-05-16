#pragma once
#include "Resource.h"

#include <vector>
#include <string>

#include "float3.h"
#include "float4x4.h"
#include "Quat.h"


struct Light
{
	std::string mType;
	float3 mColor;
	float mIntensity;
	float mRange;
	float mInnerConeAngle;
	float mOuterConeAngle;
};

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

	Light mLight;
	//Components

	int mLightId = -1;
	int mMeshId = -1;
	int mCameraId = -1;
	int mSkinId = -1;
};

class ENGINE_API ResourceModel : public Resource
{
public:

	ResourceModel(unsigned int uid);

	const std::vector<ModelNode>& GetNodes() { return modelNodes; }

	std::vector<unsigned int> mAnimationUids;

	std::vector<ModelNode> modelNodes;

	std::vector<std::pair<std::string, float4x4>> mInvBindMatrices;
};