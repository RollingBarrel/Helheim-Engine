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
        ResourceAnimation* Import(const tinygltf::Model& model, const tinygltf::Animation& animation, unsigned int uid);

        void Save(ResourceAnimation* ourAnimation);

        ResourceAnimation* Load(const char* fileName, unsigned int uid);
    }
}
