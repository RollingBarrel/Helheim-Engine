#pragma once
#include "Module.h"
#include "Globals.h"

#include "float3.h"
#include "Quat.h"

class ResourceAnimation;
class GameObject;

class AnimationController
{
public:

	AnimationController(ResourceAnimation* animation, unsigned int resource, bool loop);

	void Play(unsigned int resource, bool loop);
	void Stop();
	void Update(GameObject* model);

	void GetTransform(GameObject* model);

	float3 Interpolate(const float3& first, const float3& second, float lambda);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda);

private:

	//Time in milliseconds
	unsigned long mCurrentTime = 0;
	unsigned long mStartTime = 0;

	bool mLoop = true;

	unsigned int mResource = 0;

	ResourceAnimation* mAnimation = nullptr;
};