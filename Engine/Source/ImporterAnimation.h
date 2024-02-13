#pragma once

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"
#include <vector>
#include <Quat.h>
#include <float3.h>
#include <string> //

struct ResourceAnimation;

namespace Importer 
{
	namespace Animation 
	{
		void Import(const tinygltf::Model& model, const tinygltf::AnimationChannel& channel, ResourceAnimation* ourMesh);

		void Save(const ResourceAnimation* ourMesh);

		void Load(ResourceAnimation* ourMesh, const char* fileName);
	}

}

typedef struct Channel 
{
	std::unique_ptr<float3[]> positions;
	std::unique_ptr<float[]> posTimeStamps;
	std::unique_ptr<Quat[]> rotations;
	std::unique_ptr<float[]> rotTimeStamps;
	uint32_t numPositions = 0;
	uint32_t numRotations = 0;

};
struct ResourceAnimation 
{
	std::unordered_map<std::string,Channel*> mChannels;

};

