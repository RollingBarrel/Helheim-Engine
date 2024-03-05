#include "AnimationController.h"
#include "ResourceAnimation.h"

#include "Application.h"
#include "Timer.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

AnimationController::AnimationController(ResourceAnimation* animation, unsigned int resource, bool loop) {
	mAnimation = animation;
	mResource = resource;
	mLoop = loop;
}

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
		currentTime = std::fmod(currentTime, mAnimation->GetDuration());
	}

	//PROVISIONAL
	static float lambda;
	static int keyIndex;

	//Gets the specific channel we want
	ResourceAnimation::AnimationChannel* channel = mAnimation->GetChannels().find(name)->second;


	if (name == "translation" /* && channel->hasTranslation */) 
	{
		std::vector<float> posTimeStampsVector(channel->posTimeStamps.get(), channel->posTimeStamps.get() + channel->numPositions);
		auto upperBoundIterator = std::upper_bound(posTimeStampsVector.begin(), posTimeStampsVector.end(), currentTime);

		if (upperBoundIterator != posTimeStampsVector.end()) {
			keyIndex = std::distance(posTimeStampsVector.begin(), upperBoundIterator);

			lambda = (currentTime - channel->posTimeStamps[keyIndex-1]) / (channel->posTimeStamps[keyIndex] - channel->posTimeStamps[keyIndex-1]);
		}
		else {
			keyIndex = channel->numPositions - 1;
			lambda = 1;
		}


		pos = Interpolate(channel->positions[keyIndex-1], channel->positions[keyIndex], lambda);
	}
	else if (name == "rotation" /* && channel->hasRotation */ )
	{
		//Conversion of std::unique_ptr<float[]> to std::vector<float>
		std::vector<float> rotTimeStampsVector(channel->rotTimeStamps.get(), channel->rotTimeStamps.get() + channel->numRotations);
		//Iterating using std::upper_bound to fins the first higher number than currentTime in an ordered array
		auto upperBoundIterator = std::upper_bound(rotTimeStampsVector.begin(), rotTimeStampsVector.end(), currentTime);

		//If an upper bound has been found
		if (upperBoundIterator != rotTimeStampsVector.end()) {
			//Distance between the first element of the vector and the first higher element, aka the position of the first higher element
			keyIndex = std::distance(rotTimeStampsVector.begin(), upperBoundIterator);

			//Calculating lambda 
			lambda = (currentTime - channel->rotTimeStamps[keyIndex-1]) / (channel->rotTimeStamps[keyIndex] - channel->rotTimeStamps[keyIndex-1]);
		}
		//In case there is no upper bound
		else {
			//The index is the last element
			keyIndex = channel->numRotations - 1;
			lambda = 1;
		}


		rot = Interpolate(channel->rotations[keyIndex - 1], channel->rotations[keyIndex], lambda);
	}
	//else if (name == "scale") {
	//}
	else {
		return;
	}
}