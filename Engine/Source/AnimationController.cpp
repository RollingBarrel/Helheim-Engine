#include "AnimationController.h"
#include "ResourceAnimation.h"

#include "Application.h"
#include "GameObject.h"
#include "Timer.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

#include "Globals.h"

AnimationController::AnimationController(ResourceAnimation* animation, unsigned int resource, bool loop) {
	mAnimation = animation;
	mResource = resource;
	mLoop = loop;

	mCurrentTime = 0;
	mStartTime = 0.0f;
	mEndTime = animation->GetDuration();
}

AnimationController::AnimationController(ResourceAnimation* animation, unsigned int resource, bool loop, float startTime, float endTime) : AnimationController(animation, resource, loop)
{
	mStartTime = startTime;
	mEndTime = endTime;
}

void AnimationController::Update(GameObject* model)
{
	mCurrentTime += App->GetRealDt();
	LOG("Current Timer Time: %f", mCurrentTime);
	GetTransform(model);
}

void AnimationController::Restart()
{
	mCurrentTime = 0;
}

float3 AnimationController::Interpolate(const float3& first, const float3& second, float lambda)
{
	return first * (1.0f - lambda) + second * lambda;
}

Quat AnimationController::Interpolate(const Quat& first, const Quat& second, float lambda)
{
	Quat result;
	float dot = first.Dot(second);
	if (dot >= 0.0f) // Interpolate through the shortest path
	{
		result.x = first.x * (1.0f - lambda) + second.x * lambda;
		result.y = first.y * (1.0f - lambda) + second.y * lambda;
		result.z = first.z * (1.0f - lambda) + second.z * lambda;
		result.w = first.w * (1.0f - lambda) + second.w * lambda;
	}
	else
	{
		result.x = first.x * (1.0f - lambda) - second.x * lambda;
		result.y = first.y * (1.0f - lambda) - second.y * lambda;
		result.z = first.z * (1.0f - lambda) - second.z * lambda;
		result.w = first.w * (1.0f - lambda) - second.w * lambda;
	}
	result.Normalize();
	return result;
}

void AnimationController::SetStartTime(float time)
{
	float start = std::max(time, 0.0f);
	mStartTime = std::min(start, mEndTime);
	mCurrentTime = mStartTime;

}

void AnimationController::SetEndTime(float time)
{
	float end = std::min(time, mAnimation->GetDuration());
	mEndTime = std::max(end, mStartTime);

}

void AnimationController::GetTransform(GameObject* model)
{
	//Checks and gets the channel we want
	std::string name = model->GetName();
	//LOG("%s", name.c_str());
	ResourceAnimation::AnimationChannel* newChannel = mAnimation->GetChannel(name);

	if (newChannel != nullptr)
	{

		ResourceAnimation::AnimationChannel* channel = mAnimation->GetChannels().find(model->GetName())->second;
		if (channel == nullptr) {
			return;
		}

		if (mCurrentTime < mStartTime)
		{
			mCurrentTime = mStartTime;
		}

		//In case the current time is greater than the animation durationt, if he animation loops we change the time so it's in range
		if (mCurrentTime >= mEndTime)
		{
			if (mLoop)
			{
				mCurrentTime = std::fmod(mCurrentTime, mEndTime - mStartTime) + mStartTime;
			}
			else
			{
				mCurrentTime = mEndTime;
			}
		}

		static float lambda;
		static int keyIndex;

		if (channel->hasTranslation)
		{
			std::vector<float> posTimeStampsVector(channel->posTimeStamps.get(), channel->posTimeStamps.get() + channel->numPositions);
			auto upperBoundIterator = std::upper_bound(posTimeStampsVector.begin(), posTimeStampsVector.end(), mCurrentTime);

			if (upperBoundIterator != posTimeStampsVector.end()) {
				keyIndex = std::distance(posTimeStampsVector.begin(), upperBoundIterator);

				lambda = (mCurrentTime - channel->posTimeStamps[keyIndex - 1]) / (channel->posTimeStamps[keyIndex] - channel->posTimeStamps[keyIndex - 1]);
			}
			else {
				keyIndex = channel->numPositions - 1;
				lambda = 1;
			}

			model->SetPosition(Interpolate(channel->positions[keyIndex - 1], channel->positions[keyIndex], lambda));
		}
		if (channel->hasRotation)
		{
			//Conversion of std::unique_ptr<float[]> to std::vector<float>
			std::vector<float> rotTimeStampsVector(channel->rotTimeStamps.get(), channel->rotTimeStamps.get() + channel->numRotations);
			//Iterating using std::upper_bound to fins the first higher number than currentTime in an ordered array
			auto upperBoundIterator = std::upper_bound(rotTimeStampsVector.begin(), rotTimeStampsVector.end(), mCurrentTime);

			//If an upper bound has been found
			if (upperBoundIterator != rotTimeStampsVector.end()) {
				//Distance between the first element of the vector and the first higher element, aka the position of the first higher element
				keyIndex = std::distance(rotTimeStampsVector.begin(), upperBoundIterator);

				//Calculating lambda 
				lambda = (mCurrentTime - channel->rotTimeStamps[keyIndex - 1]) / (channel->rotTimeStamps[keyIndex] - channel->rotTimeStamps[keyIndex - 1]);
			}
			//In case there is no upper bound
			else {
				//The index is the last element
				keyIndex = channel->numRotations - 1;
				lambda = 1;
			}


			model->SetRotation(Interpolate(channel->rotations[keyIndex - 1], channel->rotations[keyIndex], lambda));
		}
		//else if (name == "scale") {
		//}
		else { return; }

		model->RecalculateMatrices();
	}
	for (const auto& child : model->GetChildren())
	{
		GetTransform(child);
	}
}