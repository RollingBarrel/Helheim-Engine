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

	std::vector<unsigned int> mAnimationUids;

private:

	ModelNode mRoot;
};

/*
#pragma once
#include "Resource.h"
#include "tiny_gltf.h"

#include "ImporterModel.h"

#include "float3.h"
#include "Quat.h"
#include <vector>

class ResourceModel : public Resource
{
	typedef struct ModelIds {
		unsigned int meshUID;
		unsigned int materialUID;
	}ModelIds;

	typedef struct Node {
		const char* name;
		unsigned int parentIndex;

		//Transform
		float3 translation;
		Quat rotation;

		std::vector<ModelIds>uids;
	}Node;

public:

	ResourceModel(unsigned int uid);

	const std::vector<ModelIds>& GetUids(Node node) const { return node.uids; }
	void SetUids(Node node, unsigned int meshUID, unsigned int materialUID);

	const std::vector<Node> GetNodes() { return mNodes; }
	void CreateNodeArray(tinygltf::Model& model);

private:
	std::vector<Node> mNodes;
};*/
