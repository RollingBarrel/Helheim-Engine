#pragma once
#include "Resource.h"

#include <vector>

#include "ImporterModel.h"

class ResourceModel : public Resource
{
	typedef struct ModelIds {
		unsigned int meshUID;
		unsigned int materialUID;
	}ModelIds;

public: 

	ResourceModel(unsigned int uid);

	const std::vector<ModelIds>& GetUids() const { return mUids; }
	void SetUids(unsigned int meshUID, unsigned int materialUID);

private:

	std::vector<ModelIds>mUids;
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
