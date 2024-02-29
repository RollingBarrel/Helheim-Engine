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

void Importer::Animation::Import(const tinygltf::Model& model, const tinygltf::Animation& animation, ResourceAnimation* ourAnimation) {

    for (const auto& srcChannel : animation.channels)
    {
        ResourceAnimation::AnimationChannel* ourChannel = new ResourceAnimation::AnimationChannel;
        std::string name = model.nodes[srcChannel.target_node].name;
        if (ourAnimation->channels.find(name) == ourAnimation->channels.end())
        {
           
        ourAnimation->addChannels(model, animation, srcChannel, ourAnimation, ourChannel);


        for (const auto& srcChannel2 : animation.channels)
        {

            if (srcChannel2.target_node == srcChannel.target_node && ourChannel->hasTranslation == false || ourChannel->hasRotation == false)
            {
                ourAnimation->addChannels(model, animation, srcChannel, ourAnimation, ourChannel);
            }

        }
        //animation -> mUID = math::LCG().Int();

         ourAnimation->channels[model.nodes[srcChannel.target_node].name] = ourChannel;
        }
       
        delete ourChannel;
        ourChannel = nullptr;  
    }

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

    if (channel.target_path == "translation") {
        for (size_t i = 0; i < numKeyframes; ++i) {
            size_t translationIndex = i * inputAccessor.count;
            
            ourChannel->positions = std::make_unique<float3[]>(numKeyframes);
            ourChannel->posTimeStamps = std::make_unique<float[]>(numKeyframes);
            ourChannel->positions[i] = float3(outputPtr[translationIndex], outputPtr[translationIndex + 1], outputPtr[translationIndex + 2]); // Store the position for each keyframe
            ourChannel->posTimeStamps[i] = inputPtr[i]; // Store the time stamp for each keyframe
            ourChannel->numPositions++;
            ourChannel->hasTranslation = true;

        } 
    }
    else if (channel.target_path == "rotation") {
        for (size_t i = 0; i < numKeyframes; ++i) {
            size_t rotationIndex = i * inputAccessor.count;
            ourChannel->rotations = std::make_unique<Quat[]>(numKeyframes);
            ourChannel->rotTimeStamps = std::make_unique<float[]>(numKeyframes);
            ourChannel->rotations[i] = Quat(outputPtr[rotationIndex], outputPtr[rotationIndex + 1], outputPtr[rotationIndex + 2], outputPtr[rotationIndex + 3]); // Store the rotation for each keyframe
            ourChannel->rotTimeStamps[i] = inputPtr[i]; // Store the time stamp for each keyframe
            ourChannel->numRotations++;
            ourChannel->hasRotation = true;
        }
     }
    
}
//void Importer::Animation::Save(const ResourceAnimation* ourAnimation) {
//    if (!ourAnimation)
//        return;
//
//    size_t totalSize = sizeof(unsigned int) * 6; // header: numPositions, numRotations, numChannels, duration, mUID size, name size
//
//    // Calculate the size of name and duration
//    totalSize += ourAnimation->name.size() + 1; 
//    totalSize += sizeof(float);
//
//    // Calculate size needed for positions
//    for (const auto& channel : ourAnimation->channels) {
//        totalSize += sizeof(unsigned int) * channel.second.numPositions;
//        totalSize += sizeof(float) * 3 * channel.second.numPositions; 
//    }
//
//    // Calculate size needed for rotations
//    for (const auto& channel : ourAnimation->channels) {
//        totalSize += sizeof(unsigned int) * channel.second.numRotations; 
//        totalSize += sizeof(float) * 4 * channel.second.numRotations;
//    }
//
//    char* fileBuffer = new char[totalSize];
//    char* cursor = fileBuffer;
//
//    //header: numPositions, numRotations, numChannels, duration, mUID size, name size
//    unsigned int header[6] = { 0 };
//    header[0] = ourAnimation->channels.empty() ? 0 : ourAnimation->channels.begin()->second.numPositions;
//    header[1] = ourAnimation->channels.empty() ? 0 : ourAnimation->channels.begin()->second.numRotations;
//    header[2] = static_cast<unsigned int>(ourAnimation->channels.size());
//    header[3] = ourAnimation->duration;
//    header[4] = sizeof(unsigned int);
//    header[5] = ourAnimation->name.size() + 1; 
//    memcpy(cursor, header, sizeof(unsigned int) * 6);
//    cursor += sizeof(unsigned int) * 6;
//
//    memcpy(cursor, &(ourAnimation->mUID), sizeof(unsigned int));
//    cursor += sizeof(unsigned int);
//
//    strcpy_s(cursor, ourAnimation->name.size() + 1, ourAnimation->name.c_str());
//    cursor += ourAnimation->name.size() + 1;
//
//    memcpy(cursor, &(ourAnimation->duration), sizeof(float));
//    cursor += sizeof(float);
//
//    // Save position data
//    for (const auto& channel : ourAnimation->channels) {
//        if (channel.second.numPositions > 0) {
//            memcpy(cursor, channel.second.posTimeStamps.get(), sizeof(unsigned int) * channel.second.numPositions);
//            cursor += sizeof(unsigned int) * channel.second.numPositions;
//            memcpy(cursor, channel.second.positions.get(), sizeof(float) * 3 * channel.second.numPositions);
//            cursor += sizeof(float) * 3 * channel.second.numPositions;
//        }
//    }
//
//    // Save rotation data
//    for (const auto& channel : ourAnimation->channels) {
//        if (channel.second.numRotations > 0) {
//            memcpy(cursor, channel.second.rotTimeStamps.get(), sizeof(unsigned int) * channel.second.numRotations);
//            cursor += sizeof(unsigned int) * channel.second.numRotations;
//            memcpy(cursor, channel.second.rotations.get(), sizeof(float) * 4 * channel.second.numRotations);
//            cursor += sizeof(float) * 4 * channel.second.numRotations;
//        }
//    }
//
//    // Construct file path
//    std::string path = LIBRARY_ANIMATION_PATH;
//    path += std::to_string(ourAnimation->mUID);
//    path += ".animaz";
//
//    App->GetFileSystem()->Save(path.c_str(), fileBuffer, totalSize);
//
//    delete[] fileBuffer;
//    fileBuffer = nullptr;
//
//}
//
//void Importer::Animation::Load(ResourceAnimation* ourAnimation, const char* fileName) {
//    if (!ourAnimation || !fileName)
//        return;
//
//    // Construct the file path
//    std::string path = LIBRARY_ANIMATION_PATH;
//    path += std::to_string(ourAnimation->mUID);
//    path += ".animaz";
//
//    // Load the file from the specified path
//    char* fileData;
//    size_t fileSize;
//
//
//    if (!App->GetFileSystem()->Load(path.c_str(), &fileData)) {
//        // If file loading fails, return nullptr
//        return;
//    }
//
//    // Read animation information from the loaded file
//    size_t offset = 0;
//
//    // Read header information
//    unsigned int numPositions = *reinterpret_cast<unsigned int*>(fileData + offset);
//    offset += sizeof(unsigned int);
//    unsigned int numRotations = *reinterpret_cast<unsigned int*>(fileData + offset);
//    offset += sizeof(unsigned int);
//    unsigned int numChannels = *reinterpret_cast<unsigned int*>(fileData + offset);
//    offset += sizeof(unsigned int);
//    float duration = *reinterpret_cast<float*>(fileData + offset);
//    offset += sizeof(float);
//
//    // Read animation UID
//    unsigned int mUID = *reinterpret_cast<unsigned int*>(fileData + offset);
//    offset += sizeof(unsigned int);
//
//    // Read animation name
//    std::string name = fileData + offset;
//    offset += name.size() + 1;
//
//    // Set animation details to the provided ourAnimation
//    ourAnimation->mUID = mUID;
//    ourAnimation->name = name;
//    ourAnimation->duration = duration;
//
//    // Read position and rotation data for each animation channel
//    for (const auto& channel : ourAnimation->channels) {
//        // Read position data if present
//        if (numPositions > 0) {
//            // Read position timestamps
//            unsigned int* posTimestamps = reinterpret_cast<unsigned int*>(fileData + offset);
//            offset += sizeof(unsigned int) * numPositions;
//
//            // Read positions
//            float3* positions = reinterpret_cast<float3*>(fileData + offset);
//            offset += sizeof(float) * 3 * numPositions;
//
//            // Add position data to animation channel
//            for (unsigned int j = 0; j < numPositions; ++j) {
//                channel.second.posTimeStamps[j] = posTimestamps[j];
//                channel.second.positions[j] = positions[j];
//            }
//        }
//
//        // Read rotation data if present
//        if (numRotations > 0) {
//            // Read rotation timestamps
//            unsigned int* rotTimestamps = reinterpret_cast<unsigned int*>(fileData + offset);
//            offset += sizeof(unsigned int) * numRotations;
//
//            // Read rotations
//            Quat* rotations = reinterpret_cast<Quat*>(fileData + offset);
//            offset += sizeof(float) * 4 * numRotations;
//
//            // Add rotation data to animation channel
//            for (unsigned int j = 0; j < numRotations; ++j) {
//                channel.second.rotTimeStamps[j] = rotTimestamps[j];
//                channel.second.rotations[j] = rotations[j];
//            }
//        }
//    }
//
//    // Deallocate file buffer
//    delete[] fileData;
//
//    // Once animation data is loaded, you can perform further operations or assign it to a resource manager
//}
