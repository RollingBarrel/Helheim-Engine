#include "AnimationController.h"
#include "ResourceAnimation.h"

#include "Application.h"
#include "Timer.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

void AnimationController::Play(unsigned int resource, bool loop)
{
	mStartTime = App->GetGameClock()->GetTotalTime();
}

void AnimationController::Update()
{
	mCurrentTime = App->GetGameClock()->GetTotalTime() - mStartTime;


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

void AnimationController::GetTransform(char* name, float3& pos, Quat& rot)
{
	//Milliseconds to seconds
	float currentTime = mCurrentTime / 1000.0f;
	//In case the animation loops, if the current time is greater than the animation duration, we change the time so it's in range
	if (mLoop) {
		float duration = 1.0f;
		currentTime = std::fmod(currentTime, mResource->GetDuration());
	}

	//PROVISIONAL

	static float lambda;

	//Gets the channels
	const std::unordered_map<std::string, ResourceAnimation::AnimationChannel*>& channels = mResource->GetChannels();

	//Gets the specific channel we want
	ResourceAnimation::AnimationChannel* channel = channels.find(name)->second;


	if (name == "translation" /* && channel->hasTranslation */) 
	{
		std::vector<float> posTimeStampsVector(channel->posTimeStamps.get(), channel->posTimeStamps.get() + channel->numPositions);
		auto upperBoundIterator = std::upper_bound(posTimeStampsVector.begin(), posTimeStampsVector.end(), currentTime);

		int keyIndex = std::distance(posTimeStampsVector.begin(), upperBoundIterator);

		if (upperBoundIterator != posTimeStampsVector.end()) {
			lambda = (currentTime - channel->posTimeStamps[keyIndex]) / (channel->posTimeStamps[keyIndex + 1] - channel->posTimeStamps[keyIndex]);
		}
		else {
			lambda = posTimeStampsVector.back();
		}


		pos = Interpolate(channel->positions[keyIndex-1], channel->positions[keyIndex], lambda);
	}
	else if (name == "rotation" /* && channel->hasRotation */ )
	{
		std::vector<float> rotTimeStampsVector(channel->rotTimeStamps.get(), channel->rotTimeStamps.get() + channel->numRotations);
		auto upperBoundIterator = std::upper_bound(rotTimeStampsVector.begin(), rotTimeStampsVector.end(), currentTime);

		int keyIndex = std::distance(rotTimeStampsVector.begin(), upperBoundIterator);

		if (upperBoundIterator != rotTimeStampsVector.end()) {
			lambda = (currentTime - channel->rotTimeStamps[keyIndex]) / (channel->rotTimeStamps[keyIndex + 1] - channel->rotTimeStamps[keyIndex]);
		}
		else {
			lambda = rotTimeStampsVector.back();
		}


		rot = Interpolate(channel->rotations[keyIndex - 1], channel->rotations[keyIndex], lambda);
	}
	//else if (name == "scale") {
	//}
	else {
		return;
	}

	//auto upperBoundIterator1 = std::upper_bound(timestamps.begin(), timestamps.end(), targetValue1);
}