#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceAnimation.h"
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

        //check if the channel already exists
        ResourceAnimation::AnimationChannel* ourChannel = ourAnimation->GetChannel(name);
        if (ourChannel == nullptr)
        {
            //create a new channel
          ResourceAnimation::AnimationChannel* ourChannel = new ResourceAnimation::AnimationChannel; 
            ourAnimation->addChannels(model, animation, srcChannel, ourAnimation, ourChannel);

            for (const auto& srcChannel2 : animation.channels)
            {

                if (srcChannel2.target_node == srcChannel.target_node && ourChannel->hasTranslation == false || ourChannel->hasRotation == false)
                {

                    ourAnimation->addChannels(model, animation, srcChannel2, ourAnimation, ourChannel);
                }
            }
            
            ourAnimation->mChannels[name] = ourChannel;

        }
        //animation -> mUID = math::LCG().Int();
         //delete ourChannel;
    }
        
    
    if (ourAnimation) {
        Importer::Animation::Save(ourAnimation);
       
    }

}



void Importer::Animation::Save (ResourceAnimation* ourAnimation)
{
    unsigned int header[] = {(ourAnimation->GetChannels().size()), ourAnimation->GetDuration()};

    unsigned int size = sizeof(header) ;
    for (const auto& channel : ourAnimation->GetChannels()) {
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

    for (const auto& channel : ourAnimation->GetChannels()) {
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

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(ourAnimation->GetUID(), true);
    App->GetFileSystem()->Save(libraryPath, fileBuffer, size);

    delete[] libraryPath;
    delete[] fileBuffer;
}

ResourceAnimation* Importer::Animation::Load(const char* filePath, unsigned int uid)
{
    char* fileBuffer = nullptr;
    ResourceAnimation* ourAnimation = nullptr;

    if (App->GetFileSystem()->Load(filePath, &fileBuffer))
    {
        // Load Header
        char* cursor = fileBuffer;
        unsigned int header[2];
        unsigned int bytes = sizeof(header);
        memcpy(header, cursor, bytes);
        cursor += bytes;
        unsigned int numChannels = header[0];
        float duration = *reinterpret_cast<float*>(cursor);
        cursor += sizeof(float);

        ourAnimation = new ResourceAnimation(uid, "");

        // Load Channels
        for (unsigned int i = 0; i < numChannels; ++i)
        {
            unsigned int nodeNameSize;
            memcpy(&nodeNameSize, cursor, sizeof(unsigned int));
            cursor += sizeof(unsigned int);

            std::string nodeName(cursor, nodeNameSize);
            cursor += nodeNameSize;

            ResourceAnimation::AnimationChannel* channel = new ResourceAnimation::AnimationChannel;

            float* posTimeStamps = nullptr;
            float* positions = nullptr;
            float* rotTimeStamps = nullptr;
            float* rotations = nullptr;

            if (channel->hasTranslation)
            {
                bytes = sizeof(float) * channel->numPositions;
                posTimeStamps = new float[channel->numPositions];
                memcpy(posTimeStamps, cursor, bytes);
                cursor += bytes;

                bytes = sizeof(float) * 3 * channel->numPositions;
                positions = new float[3 * channel->numPositions];
                memcpy(positions, cursor, bytes);
                cursor += bytes;
            }

            if (channel->hasRotation)
            {
                bytes = sizeof(float) * channel->numRotations;
                rotTimeStamps = new float[channel->numRotations];
                memcpy(rotTimeStamps, cursor, bytes);
                cursor += bytes;

                bytes = sizeof(float) * 4 * channel->numRotations;
                rotations = new float[4 * channel->numRotations];
                memcpy(rotations, cursor, bytes);
                cursor += bytes;
            }
            
            ourAnimation->mChannels[nodeName] = channel;


        }

        delete[] fileBuffer;
    }

    return ourAnimation;
}








// OLD ANIMATION LOAD
//void Importer::Animation::Load(ResourceAnimation* ourAnimation, const char* fileName) {
//    if (!ourAnimation || !fileName)
//        return;
//
//    char* fileBuffer = nullptr;
//    ResourceImp* rMesh = nullptr;
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
