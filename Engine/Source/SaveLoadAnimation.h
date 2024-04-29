#pragma once
#include "Globals.h"
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
        ENGINE_API void Save(ResourceAnimation* ourAnimation);
        ENGINE_API ResourceAnimation* Load(const char* fileName, unsigned int uid);
    }
}