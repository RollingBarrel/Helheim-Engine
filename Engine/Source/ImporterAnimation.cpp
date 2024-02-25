#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"

#include "ImporterAnimation.h"

#include "glew.h"

#include "float2.h"
#include "float3.h"
#include "float4.h"

#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"


void Importer::Animation::Import(const tinygltf::Model& model, const tinygltf::AnimationChannel& channel, ResourceAnimation* ourAnimation) {
    if (!ourAnimation)
        return;

    // Get the index of the animation sampler from the current channel
    const int samplerIndex = channel.sampler;

    const tinygltf::AnimationSampler& animationSampler = model.animations[channel.target_node].samplers[samplerIndex];
    const int inputAccessorIndex = animationSampler.input;
    const int outputAccessorIndex = animationSampler.output;

    const tinygltf::Accessor& inputAccessor = model.accessors[inputAccessorIndex];
    const tinygltf::Accessor& outputAccessor = model.accessors[outputAccessorIndex];

    // Retrieves the buffer views for both input and output data.
    const tinygltf::BufferView& inputBufferView = model.bufferViews[inputAccessor.bufferView];
    const tinygltf::BufferView& outputBufferView = model.bufferViews[outputAccessor.bufferView];

    // Retrieves the actual buffer data from the model's buffers based on the buffer views and accessors.
    const unsigned char* inputBuffer = &model.buffers[inputBufferView.buffer].data[inputBufferView.byteOffset + inputAccessor.byteOffset];
    const unsigned char* outputBuffer = &model.buffers[outputBufferView.buffer].data[outputBufferView.byteOffset + outputAccessor.byteOffset];

    const float* inputPtr = reinterpret_cast<const float*>(inputBuffer);
    const float* outputPtr = reinterpret_cast<const float*>(outputBuffer);

    // Determine the number of keyframes is the number of output accessors
    size_t numKeyframes = outputAccessor.count;

    // Create an animation channel
    ResourceAnimation::AnimationChannel animChannel;

    // Assuming each keyframe has 3 floats for translation (x, y, z)
    // and each keyframe has 4 floats for rotation (x, y, z, w)
    for (size_t i = 0; i < numKeyframes; ++i) {
        size_t translationIndex = i * inputAccessor.count;
        size_t rotationIndex = i * inputAccessor.count;

        if (channel.target_path == "translation") {
            animChannel.positions = std::make_unique<float3[]>(numKeyframes);
            animChannel.posTimeStamps = std::make_unique<float[]>(numKeyframes);
            animChannel.positions[i] = float3(outputPtr[translationIndex], outputPtr[translationIndex + 1], outputPtr[translationIndex + 2]); // Store the position for each keyframe
            animChannel.posTimeStamps[i] = inputPtr[i]; // Store the time stamp for each keyframe
            animChannel.numPositions++;
        }
        else if (channel.target_path == "rotation") {
            animChannel.rotations = std::make_unique<Quat[]>(numKeyframes);
            animChannel.rotTimeStamps = std::make_unique<float[]>(numKeyframes);
            animChannel.rotations[i] = Quat(outputPtr[rotationIndex], outputPtr[rotationIndex + 1], outputPtr[rotationIndex + 2], outputPtr[rotationIndex + 3]); // Store the rotation for each keyframe
            animChannel.rotTimeStamps[i] = inputPtr[i]; // Store the time stamp for each keyframe
            animChannel.numRotations++;
        }
    }
    // Store animation channel in the animation
   // ourAnimation->channels[channel.target_node] = animChannel;
}
void Importer::Animation::Save(const ResourceAnimation* ourAnimation) {
    if (!ourAnimation)
        return;

    size_t totalSize = sizeof(unsigned int) * 6; // header: numPositions, numRotations, numChannels, duration, mUID size, name size

    // Calculate the size of name and duration
    totalSize += ourAnimation->name.size() + 1; 
    totalSize += sizeof(float);

    // Calculate size needed for positions
    for (const auto& channel : ourAnimation->channels) {
        totalSize += sizeof(unsigned int) * channel.second.numPositions;
        totalSize += sizeof(float) * 3 * channel.second.numPositions; 
    }

    // Calculate size needed for rotations
    for (const auto& channel : ourAnimation->channels) {
        totalSize += sizeof(unsigned int) * channel.second.numRotations; 
        totalSize += sizeof(float) * 4 * channel.second.numRotations;
    }

    char* fileBuffer = new char[totalSize];
    char* cursor = fileBuffer;

    //header: numPositions, numRotations, numChannels, duration, mUID size, name size
    unsigned int header[6] = { 0 };
    header[0] = ourAnimation->channels.empty() ? 0 : ourAnimation->channels.begin()->second.numPositions;
    header[1] = ourAnimation->channels.empty() ? 0 : ourAnimation->channels.begin()->second.numRotations;
    header[2] = static_cast<unsigned int>(ourAnimation->channels.size());
    header[3] = ourAnimation->duration;
    header[4] = sizeof(unsigned int);
    header[5] = ourAnimation->name.size() + 1; 
    memcpy(cursor, header, sizeof(unsigned int) * 6);
    cursor += sizeof(unsigned int) * 6;

    memcpy(cursor, &(ourAnimation->mUID), sizeof(unsigned int));
    cursor += sizeof(unsigned int);

    strcpy_s(cursor, ourAnimation->name.size() + 1, ourAnimation->name.c_str());
    cursor += ourAnimation->name.size() + 1;

    memcpy(cursor, &(ourAnimation->duration), sizeof(float));
    cursor += sizeof(float);

    // Save position data
    for (const auto& channel : ourAnimation->channels) {
        if (channel.second.numPositions > 0) {
            memcpy(cursor, channel.second.posTimeStamps.get(), sizeof(unsigned int) * channel.second.numPositions);
            cursor += sizeof(unsigned int) * channel.second.numPositions;
            memcpy(cursor, channel.second.positions.get(), sizeof(float) * 3 * channel.second.numPositions);
            cursor += sizeof(float) * 3 * channel.second.numPositions;
        }
    }

    // Save rotation data
    for (const auto& channel : ourAnimation->channels) {
        if (channel.second.numRotations > 0) {
            memcpy(cursor, channel.second.rotTimeStamps.get(), sizeof(unsigned int) * channel.second.numRotations);
            cursor += sizeof(unsigned int) * channel.second.numRotations;
            memcpy(cursor, channel.second.rotations.get(), sizeof(float) * 4 * channel.second.numRotations);
            cursor += sizeof(float) * 4 * channel.second.numRotations;
        }
    }

    // Construct file path
    std::string path = LIBRARY_ANIMATION_PATH;
    path += std::to_string(ourAnimation->mUID);
    path += ".anim";

    App->GetFileSystem()->Save(path.c_str(), fileBuffer, totalSize);

    delete[] fileBuffer;
    fileBuffer = nullptr;

}
