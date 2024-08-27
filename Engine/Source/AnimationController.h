#pragma once
#include "Module.h"
#include "Globals.h"

#include "float3.h"
#include "Quat.h"

#include "ResourceAnimation.h"

class GameObject;

class AnimationController
{
public:

	AnimationController(ResourceAnimation* animation);
	~AnimationController();

	void Update();
	void Restart();

	//GetTransforms
	void GetTransform(GameObject* model);
	void GetTransform_Blending(GameObject* model);
	//void GetTransform_BlendingAnimations(GameObject* model);
	void CalculateIndexAndLambda(ResourceAnimation::AnimationChannel* channel, const char* channelType, float timeToFind, int& keyIndex, float& lambda);


	bool GetLoop() const { return mLoop; }
	void SetLoop(bool loop) { mLoop = loop; }

	//Current time clip
	const float GetStartTime() const { return mStartTime; }
	void SetStartTime(float time);

	const float GetEndTime() const { return mEndTime; }
	void SetEndTime(float time);

	const float GetAnimSpeed() const { return mSpeed; }
	void SetAnimSpeed(float speed) { mSpeed = speed; }

	//Animation Resource
	void SetCurrentAnimation(ResourceAnimation* animation) { mCurrentAnimation = animation; }

	//Blending
	void SetTransitionDuration(float time) { mTransitionDuration = time; }
	void ActivateTransition() { mTransition = true; mStartTransitionTime = mCurrentTime; }
	void SetClipStartTime(float time) { mClipStartTime = time; }
	bool GetIsInTransition() const { return mTransition; }
	void EndBlending();

	float GetTransitionDuration() const { return mTransitionDuration; }


	float GetAnimationCurrentTime() const { return mCurrentTime; }
	unsigned int GetAnimationUID() const;
private:
	float3 Interpolate(const float3& first, const float3& second, float lambda);
	Quat Interpolate(const Quat& first, const Quat& second, float lambda);

	//Time in milliseconds
	float mCurrentTime = 0.0f;
	float mStartTime = 0.0f;
	float mEndTime = 0.0f;

	//Provisional to make blending work
	float mStartTransitionTime = 0.0f;
	float mTransitionDuration = 0.2f;
	bool mTransition = false;
	float mClipStartTime = 0.0f;
	float mCurrentTransitionTime = 0.0f;

	float mSpeed = 1.0;

	bool mLoop = false;

	ResourceAnimation* mCurrentAnimation = nullptr;
	unsigned int mAnimationUID = 0;
};