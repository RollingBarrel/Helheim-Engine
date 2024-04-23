#pragma once
//#include "Math/float3.h"
//#include "Math/Quat.h"
//
//struct Keyframe {
//    float time;
//    float3 position;
//    Quat rotation;
//    // Optionally include scale if needed
//    // float3 scale;
//};

class ResourceAnimation;

namespace Importer 
{
    namespace Animation 
    {
        void Save(ResourceAnimation* ourAnimation);
        ResourceAnimation* Load(const char* fileName, unsigned int uid);
    }
}