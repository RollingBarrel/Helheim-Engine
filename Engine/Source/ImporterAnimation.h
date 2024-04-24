#pragma once

//struct Keyframe {
//    float time;
//    float3 position;
//    Quat rotation;
//    // Optionally include scale if needed
//    // float3 scale;
//};

class ResourceAnimation;
namespace tinygltf 
{
    class Model;
    struct Animation;
    struct AnimationChannel;
}

namespace Importer 
{
    namespace Animation 
    {
        ResourceAnimation* Import(const tinygltf::Model& model, const tinygltf::Animation& animation, unsigned int& uid);
    }
}
