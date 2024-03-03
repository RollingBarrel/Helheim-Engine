
#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceAnimation.h"


#include "glew.h"

#include "float2.h"
#include "float3.h"
#include "float4.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"



ResourceAnimation::ResourceAnimation(
    unsigned int uid, 
    const std::string inName) : Resource(uid, Type::Animation) , mName(inName)
{
}

ResourceAnimation::~ResourceAnimation()
{
   // CleanUp();
}

ResourceAnimation::AnimationChannel* ResourceAnimation::GetChannel(const std::string& name) const
{
	auto it = mChannels.find(name);
    if (it != mChannels.end())
    {
		return it->second;
	}
	return nullptr;
}

void ResourceAnimation::addChannels(const tinygltf::Model& model, const tinygltf::Animation& animation, const tinygltf::AnimationChannel& channel, ResourceAnimation* ourAnimation, ResourceAnimation::AnimationChannel* ourChannel) {
    if (!ourAnimation)
        return;

    // Get the index of the animation sampler from the current channel
    const int samplerIndex = channel.sampler;

    const tinygltf::AnimationSampler& animationSampler = animation.samplers[samplerIndex];
    const int inputAccessorIndex = animationSampler.input;
    const int outputAccessorIndex = animationSampler.output;

    const tinygltf::Accessor& inputAccessor = model.accessors[inputAccessorIndex];
    const tinygltf::Accessor& outputAccessor = model.accessors[outputAccessorIndex];

    const tinygltf::BufferView& inputBufferView = model.bufferViews[inputAccessor.bufferView];
    const tinygltf::BufferView& outputBufferView = model.bufferViews[outputAccessor.bufferView];

    // Retrieves the actual buffer data 
    const unsigned char* inputBuffer = &model.buffers[inputBufferView.buffer].data[inputBufferView.byteOffset + inputAccessor.byteOffset];
    const unsigned char* outputBuffer = &model.buffers[outputBufferView.buffer].data[outputBufferView.byteOffset + outputAccessor.byteOffset];

    const float* inputPtr = reinterpret_cast<const float*>(inputBuffer);
    const float* outputPtr = reinterpret_cast<const float*>(outputBuffer);

    size_t numKeyframes = outputAccessor.count;

    if (channel.target_path == "translation" && ourChannel->hasTranslation == false) {
        ourChannel->positions = std::make_unique<float3[]>(numKeyframes);
        ourChannel->posTimeStamps = std::make_unique<float[]>(numKeyframes);

        for (size_t i = 0; i < numKeyframes; ++i) {
            size_t translationIndex = i * inputAccessor.count;


            ourChannel->positions[i] = float3(outputPtr[translationIndex], outputPtr[translationIndex + 1], outputPtr[translationIndex + 2]); // Store the position for each keyframe
            ourChannel->posTimeStamps[i] = inputPtr[i]; // Store the time stamp for each keyframe
            ourChannel->numPositions++;
            ourChannel->hasTranslation = true;

        }
    }
    else if (channel.target_path == "rotation" && ourChannel->hasRotation == false) {
        ourChannel->rotations = std::make_unique<Quat[]>(numKeyframes);
        ourChannel->rotTimeStamps = std::make_unique<float[]>(numKeyframes);
        for (size_t i = 0; i < numKeyframes; ++i) {
            size_t rotationIndex = i * inputAccessor.count;

            ourChannel->rotations[i] = Quat(outputPtr[rotationIndex], outputPtr[rotationIndex + 1], outputPtr[rotationIndex + 2], outputPtr[rotationIndex + 3]); // Store the rotation for each keyframe
            ourChannel->rotTimeStamps[i] = inputPtr[i]; // Store the time stamp for each keyframe
            ourChannel->numRotations++;
            ourChannel->hasRotation = true;

        }
    }
    //else if (channel.target_path == "scale" && ourChannel->hasRotation == false) {
    //    for (size_t i = 0; i < numKeyframes; ++i) {
    //        size_t scaleIndex = i * inputAccessor.count;
    //        ourChannel->scales = std::make_unique<float3[]>(numKeyframes);
    //        ourChannel->scaleTimeStamps = std::make_unique<float[]>(numKeyframes);
    //        ourChannel->scales[i] = float3(outputPtr[scaleIndex], outputPtr[scaleIndex + 1], outputPtr[scaleIndex + 2]); // Store the position for each keyframe
    //        ourChannel->scaleTimeStamps[i] = inputPtr[i]; // Store the time stamp for each keyframe
    //        ourChannel->numScales++;
    //        ourChannel->hasScale = true;

    //    }

}

void ResourceAnimation::CleanUp()
{
    mName.clear();
    mDuration = 0.0f;

    for (auto& channel : mChannels)
    {
		delete channel.second;
	}
	mChannels.clear();
}