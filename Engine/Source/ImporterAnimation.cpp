#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceAnimation.h"
#include "ImporterAnimation.h"
#include "SaveLoadAnimation.h"



#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"


static void AddChannels(ResourceAnimation* rAnim, const tinygltf::Model& model, const tinygltf::Animation& animation, const tinygltf::AnimationChannel& channel, ResourceAnimation& ourAnimation, ResourceAnimation::AnimationChannel* ourChannel) {
    if (!&ourAnimation)
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

    float duration = rAnim->mDuration;

    if (channel.target_path == "translation" && ourChannel->hasTranslation == false) {

        ourChannel->positions = std::make_unique<float3[]>(numKeyframes);
        ourChannel->posTimeStamps = std::make_unique<float[]>(numKeyframes);

        for (size_t i = 0; i < numKeyframes; ++i) {

            size_t translationIndex = i * 3;

            ourChannel->positions[i] = float3(outputPtr[translationIndex], outputPtr[translationIndex + 1], outputPtr[translationIndex + 2]); // Store the position for each keyframe
            ourChannel->posTimeStamps[i] = inputPtr[i]; // Store the time stamp for each keyframe
            ourChannel->numPositions++;

            if (duration < ourChannel->posTimeStamps[i])
                duration = ourChannel->posTimeStamps[i];
        }

        ourChannel->hasTranslation = true;

    }
    else if (channel.target_path == "rotation" && ourChannel->hasRotation == false) {

        ourChannel->rotations = std::make_unique<Quat[]>(numKeyframes);
        ourChannel->rotTimeStamps = std::make_unique<float[]>(numKeyframes);

        for (size_t i = 0; i < numKeyframes; ++i) {

            size_t rotationIndex = i * 4;

            ourChannel->rotations[i] = Quat(outputPtr[rotationIndex], outputPtr[rotationIndex + 1], outputPtr[rotationIndex + 2], outputPtr[rotationIndex + 3]); // Store the rotation for each keyframe
            ourChannel->rotTimeStamps[i] = inputPtr[i]; // Store the time stamp for each keyframe
            ourChannel->numRotations++;

            if (duration < ourChannel->rotTimeStamps[i])
                duration = ourChannel->rotTimeStamps[i];
        }

        ourChannel->hasRotation = true;
    }

    ourAnimation.mDuration = duration;
}


ResourceAnimation* Importer::Animation::Import(const tinygltf::Model& model, const tinygltf::Animation& animation, unsigned int& uid) {

    std::string rootName = animation.name;

    ResourceAnimation* rAnimation = new ResourceAnimation(uid++, rootName);

    for (const auto& srcChannel : animation.channels)
    {
        std::string name = model.nodes[srcChannel.target_node].name;

        //check if the channel already exists
        ResourceAnimation::AnimationChannel* ourChannel = rAnimation->GetChannel(name);
        if (ourChannel == nullptr)
        {
            //create a new channel
          ResourceAnimation::AnimationChannel* ourChannel = new ResourceAnimation::AnimationChannel; 
          AddChannels(rAnimation, model, animation, srcChannel, *rAnimation, ourChannel);

            for (const auto& srcChannel2 : animation.channels)
            {        
                if (srcChannel2.target_node == srcChannel.target_node && (ourChannel->hasTranslation == false || ourChannel->hasRotation == false))
                {
                    AddChannels(rAnimation, model, animation, srcChannel2, *rAnimation, ourChannel);
                }
            }
            
            rAnimation->mChannels[name] = ourChannel;

        }
        //animation -> mUID = math::LCG().Int();
         //delete ourChannel;
        
    }
        
    

    if (rAnimation) 
    {
        Importer::Animation::Save(rAnimation);
    }

    return rAnimation;
}