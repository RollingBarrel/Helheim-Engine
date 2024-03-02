#pragma once
#include "Module.h"
#include "Globals.h"

#include "Application.h"
#include "Timer.h"

#include "float3.h"
#include "Quat.h"

class AnimationController
{
public:
	unsigned int currentTime;
	bool loop;
	/*UID*/ int resource;

	void Play(/*UID*/ int resource, bool loop);
	void Stop();
	void Update();

	void GetTransform(char* name, float3& pos, Quat& rot);
	float3 Interpolate(const float3& first, const float3& second, float lambda);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda);
};