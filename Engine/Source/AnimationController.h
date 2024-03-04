#pragma once
#include "Module.h"
#include "Globals.h"

#include "float3.h"
#include "Quat.h"

class ResourceAnimation;

class AnimationController
{
public:

	void Play(unsigned int resource, bool loop);
	void Stop();
	void Update();

	//should there be a transform class?
	void GetTransform(char* name, float3& pos, Quat& rot);

	float3 Interpolate(const float3& first, const float3& second, float lambda);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda);

private:

	//Time in milliseconds
	unsigned int mCurrentTime;
	unsigned int mStartTime;

	bool mLoop;


	unsigned int mResourceUID;
	ResourceAnimation* mResource = nullptr;
};