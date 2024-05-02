#include "Globals.h"
#include "Application.h"
#include "ModuleFileSystem.h"
#include "ResourceAnimation.h"


#include "glew.h"

#include "float2.h"
#include "float3.h"
#include "float4.h"



ResourceAnimation::ResourceAnimation(
    unsigned int uid, 
    const std::string inName) : Resource(uid, Type::Animation) , mName(inName)
{
}

ResourceAnimation::~ResourceAnimation()
{
   CleanUp();
}

ResourceAnimation::AnimationChannel* ResourceAnimation::GetChannel(const std::string& name) const
{
	auto it = mChannels.find(name);
    if (it != mChannels.end())
    {
		return it->second;
	}
	return nullptr;
}

void ResourceAnimation::CleanUp()
{
    mName.clear();
    mDuration = 0;

    for (auto& channel : mChannels)
    {
        channel.second->positions.reset();
        channel.second->rotations.reset();
        channel.second->posTimeStamps.reset();
        channel.second->rotTimeStamps.reset();
        delete channel.second;
    }
    mChannels.clear();
}