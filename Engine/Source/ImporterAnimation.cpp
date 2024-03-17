#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceAnimation.h"
#include "ImporterAnimation.h"
#include "glew.h"




#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_EXTERNAL_IMAGE
#include "tiny_gltf.h"



ResourceAnimation* Importer::Animation::Import(const tinygltf::Model& model, const tinygltf::Animation& animation, unsigned int& uid) {

    std::string rootName = model.nodes[0].name;

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
          rAnimation->AddChannels(model, animation, srcChannel, *rAnimation, ourChannel);

            for (const auto& srcChannel2 : animation.channels)
            {        
                if (srcChannel2.target_node == srcChannel.target_node && (ourChannel->hasTranslation == false || ourChannel->hasRotation == false))
                {
                    rAnimation->AddChannels(model, animation, srcChannel2, *rAnimation, ourChannel);
                }
            }
            
            rAnimation->mChannels[name] = ourChannel;

        }
        //animation -> mUID = math::LCG().Int();
         //delete ourChannel;
    }
        
    

    if (rAnimation) {
        Importer::Animation::Save(rAnimation);
       
    }

    return rAnimation;
}



void Importer::Animation::Save(ResourceAnimation* ourAnimation)
{
    unsigned int header[2] = {(ourAnimation->GetChannels().size()), ourAnimation->GetDuration()};

    unsigned int size = sizeof(header);
    for (const auto& channel : ourAnimation->GetChannels()) {
        size += sizeof(unsigned int) + channel.first.length() + 1;

        size += sizeof(bool);
        if (channel.second->hasTranslation) {
            size += sizeof(unsigned int);
            size += sizeof(float) * 3 * channel.second->numPositions; 
            size += sizeof(float) * channel.second->numPositions;
        }
        size += sizeof(bool);
        if (channel.second->hasRotation) {

            size += sizeof(unsigned int);
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

        unsigned int nodeNameSize = channel.first.length() + 1;
        memcpy(cursor, &nodeNameSize, sizeof(unsigned int));
        cursor += sizeof(unsigned int);
        memcpy(cursor, channel.first.data(), nodeNameSize);
        cursor += nodeNameSize;

        bytes = sizeof(bool);
        memcpy(cursor, &channel.second->hasTranslation, bytes);
        cursor += bytes;

        if (channel.second->hasTranslation) {
            bytes = sizeof(unsigned int);
            memcpy(cursor, &channel.second->numPositions, bytes);
            cursor += bytes;
            bytes = sizeof(float) * channel.second->numPositions;
            memcpy(cursor, channel.second->posTimeStamps.get(), bytes);
            cursor += bytes;
            bytes = sizeof(float) * 3 * channel.second->numPositions;
            memcpy(cursor, channel.second->positions.get(), bytes);
            cursor += bytes;
        }

        bytes = sizeof(bool);
        memcpy(cursor, &channel.second->hasRotation, bytes);
        cursor += bytes;
        if (channel.second->hasRotation) {
            bytes = sizeof(unsigned int);
            memcpy(cursor, &channel.second->numRotations, bytes);
            cursor += bytes;
            bytes = sizeof(float) * channel.second->numRotations;
            memcpy(cursor, channel.second->rotTimeStamps.get(), bytes);
            cursor += bytes;
            bytes = sizeof(float) * 4 * channel.second->numRotations;
            memcpy(cursor, channel.second->rotations.get(), bytes);
            cursor += bytes;
        }
    }

    const char* libraryPath = App->GetFileSystem()->GetLibraryFile(ourAnimation->GetUID(), true);
    LOG("Animation:");
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
        ourAnimation = new ResourceAnimation(uid, "");

        // Load Header
        char* cursor = fileBuffer;
        unsigned int header[2];
        unsigned int bytes = sizeof(header);
        memcpy(header, cursor, bytes);
        cursor += bytes;
        unsigned int numChannels = header[0];
        ourAnimation->mDuration = header[1];

        // Load Channels
        for (unsigned int i = 0; i < numChannels; ++i)
        {
            unsigned int nodeNameSize = 0;
            memcpy(&nodeNameSize, cursor, sizeof(unsigned int));
            cursor += sizeof(unsigned int);

            char* name = new char[nodeNameSize];
            memcpy(name, cursor, nodeNameSize);
            cursor += nodeNameSize;

            ResourceAnimation::AnimationChannel* channel = new ResourceAnimation::AnimationChannel;

            float* posTimeStamps = nullptr;
            float3* positions = nullptr;
            float* rotTimeStamps = nullptr;
            Quat* rotations = nullptr;

            bytes = sizeof(bool);
            memcpy(&channel->hasTranslation, cursor, bytes);
            cursor += bytes;

            if (channel->hasTranslation)
            {
                bytes = sizeof(unsigned int);
                memcpy(&channel->numPositions, cursor, bytes);
                cursor += bytes;

                bytes = sizeof(float) * channel->numPositions;
                posTimeStamps = new float[channel->numPositions];
                memcpy(posTimeStamps, cursor, bytes);
                cursor += bytes;
                channel->posTimeStamps.reset(posTimeStamps);

                bytes = sizeof(float) * 3 * channel->numPositions;
                positions = new float3[channel->numPositions];
                memcpy(positions, cursor, bytes);
                cursor += bytes;
                channel->positions.reset(positions);
            }

            bytes = sizeof(bool);
            memcpy(&channel->hasRotation, cursor, bytes);
            cursor += bytes;

            if (channel->hasRotation)
            {
                bytes = sizeof(unsigned int);
                memcpy(&channel->numRotations, cursor, bytes);
                cursor += bytes;

                bytes = sizeof(float) * channel->numRotations;
                rotTimeStamps = new float[channel->numRotations];
                memcpy(rotTimeStamps, cursor, bytes);
                cursor += bytes;
                channel->rotTimeStamps.reset(rotTimeStamps);

                bytes = sizeof(float) * 4 * channel->numRotations;
                rotations = new Quat[ channel->numRotations];
                memcpy(rotations, cursor, bytes);
                cursor += bytes;
                channel->rotations.reset(rotations);
            }
            
            ourAnimation->mChannels[name] = channel;
        }

        delete[] fileBuffer;
    }

    return ourAnimation;
}

