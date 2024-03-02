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
        
        std::string name = model.nodes[srcChannel.target_node].name;
        if (ourAnimation->channels.find(name) == ourAnimation->channels.end())
        {
          ResourceAnimation::AnimationChannel* ourChannel = new ResourceAnimation::AnimationChannel; 
        ourAnimation->addChannels(model, animation, srcChannel, ourAnimation, ourChannel);


        for (const auto& srcChannel2 : animation.channels)
        {

            if (srcChannel2.target_node == srcChannel.target_node && ourChannel->hasTranslation == false || ourChannel->hasRotation == false)
            {

                ourAnimation->addChannels(model, animation, srcChannel2, ourAnimation, ourChannel);
            }

        }
        //animation -> mUID = math::LCG().Int();
       
        ourAnimation->channels[name]=ourChannel;
          //delete ourChannel;
        }
        
    }

    
    if (ourAnimation) {
        Importer::Animation::Save(ourAnimation);
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
    


void Importer::Animation::Save(const ResourceAnimation* ourAnimation)
{
    unsigned int header[] = { ourAnimation->channels.size(), ourAnimation->duration };

    unsigned int size = sizeof(header) ;
    for (const auto& channel : ourAnimation->channels) {
        size += sizeof(unsigned int) + channel.first.size();

        if (channel.second->hasTranslation) {
            size += sizeof(float) * 3 * channel.second->numPositions; 
            size += sizeof(float) * channel.second->numPositions;
        }
        if (channel.second->hasRotation) {
            size += sizeof(float) * 4 * channel.second->numRotations; 
            size += sizeof(float) * channel.second->numRotations;
        }
    }

    char* fileBuffer = new char[size];
    char* cursor = fileBuffer;

    unsigned int bytes = sizeof(header);
    memcpy(cursor, header, bytes);
    cursor += bytes;

    for (const auto& channel : ourAnimation->channels) {
        unsigned int nodeNameSize = static_cast<unsigned int>(channel.first.size());
        memcpy(cursor, &nodeNameSize, sizeof(unsigned int));
        cursor += sizeof(unsigned int);
        memcpy(cursor, channel.first.data(), nodeNameSize);
        cursor += nodeNameSize;

        if (channel.second->hasTranslation) {
            bytes = sizeof(float) * channel.second->numPositions;
            memcpy(cursor, channel.second->posTimeStamps.get(), bytes);
            cursor += bytes;
            bytes = sizeof(float) * 3 * channel.second->numPositions;
            memcpy(cursor, channel.second->positions.get(), bytes);
            cursor += bytes;
        }

        if (channel.second->hasRotation) {
            bytes = sizeof(float) * channel.second->numRotations;
            memcpy(cursor, channel.second->rotTimeStamps.get(), bytes);
            cursor += bytes;
            bytes = sizeof(float) * 4 * channel.second->numRotations;
            memcpy(cursor, channel.second->rotations.get(), bytes);
            cursor += bytes;
        }
    }

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(ourAnimation->mUID, true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}

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
