#pragma once

#include <memory> // Include the <memory> header for std::unique_ptr
#include <unordered_map>
#include "tiny_gltf.h"
#include "Quat.h" // Include the headers for Quat and float3 if they are custom types
#include "float3.h"
#include <string>


struct Keyframe {
    float time;
    float3 position;
    Quat rotation;
    // Optionally include scale if needed
    // float3 scale;
};

struct ResourceAnimation;

namespace Importer {
    namespace Animation {
        void Import(const tinygltf::Model& model, const tinygltf::Animation& animation, ResourceAnimation* ourAnimation);

        void ImportChannel(const tinygltf::Model& model, const tinygltf::AnimationChannel& channel, ResourceAnimation* ourAnimation);

        void Save(const ResourceAnimation* ourMesh);

        void Load(ResourceAnimation* ourMesh, const char* fileName);
    }
}

class ResourceAnimation {
public:
    std::string name; 
    float duration;   
    unsigned int mUID = 0;

    // Define a structure to store keyframes for positions and rotations
    struct AnimationChannel {
        std::unique_ptr<float3[]> positions;
        std::unique_ptr<float[]> posTimeStamps;
        std::unique_ptr<Quat[]> rotations;
        std::unique_ptr<float[]> rotTimeStamps;
        uint32_t numPositions = 0;
        uint32_t numRotations = 0;

    };

    // Use unordered_map to efficiently store animation channels by node name
    std::unordered_map<std::string, AnimationChannel> channels;

};