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
	AnimationController(ResourceAnimation* animation, unsigned int resource, bool loop, float startTime, float endTime);

	void Update(GameObject* model);
	void Restart();

	void GetTransform(GameObject* model);
	void GetTransformBlending(GameObject* model, float newClipStartTime);

	float3 Interpolate(const float3& first, const float3& second, float lambda);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda);

	bool GetLoop() { return mLoop; }
	void SetLoop(bool loop) { mLoop = loop; }

	const float GetStartTime() const { return mStartTime; }
	void SetStartTime(float time);

	const float GetEndTime() const { return mEndTime; }
	void SetEndTime(float time);

	const float GetAnimSpeed() const { return mSpeed; }
	void SetAnimSpeed(float speed) { mSpeed = speed; }


private:

	//Time in milliseconds
	float mCurrentTime = 0;
	float mStartTime = 0;
	float mEndTime = 0;

	//Provisional to make blending work
	float mStartTransitionTime = 0;
	float mTrasitionDuration = 1;
	bool mTransition = false;
	float newClipStartTime = 0;

	float mSpeed = 1.0;

	bool mLoop = true;

	unsigned int mResource;

	ResourceAnimation* mAnimation = nullptr;
};