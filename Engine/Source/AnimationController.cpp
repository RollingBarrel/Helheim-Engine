#include "AnimationController.h"

#include "Application.h"
#include "GameObject.h"
#include "Timer.h"
#include "ModuleResource.h"

#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>

#include "Globals.h"

AnimationController::AnimationController(ResourceAnimation* animation,  bool loop) {
	mCurrentAnimation = animation;
	mLoop = loop;

	mCurrentTime = 0;
	mStartTime = 0.0f;
	mEndTime = animation->GetDuration();
}

AnimationController::AnimationController(ResourceAnimation* animation,  bool loop, float startTime, float endTime) : AnimationController(animation, loop)
{
	mStartTime = startTime;
	mEndTime = endTime;
}

AnimationController::~AnimationController()
{
	if (mCurrentAnimation)
	{
		App->GetResource()->ReleaseResource(mCurrentAnimation->GetUID());
	}
	if (mNextAnimation)
	{
		App->GetResource()->ReleaseResource(mNextAnimation->GetUID());
	}
}

void AnimationController::Update()
{
	mCurrentTime += App->GetDt() * mSpeed;
	
}

void AnimationController::Restart()
{
	mCurrentTime = mStartTime;
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
	mStartTime = std::max(time, 0.0f);

}

void AnimationController::SetEndTime(float time)
{
	mEndTime = std::min(time, mCurrentAnimation->GetDuration());

}

void AnimationController::GetTransform(GameObject* model)
{
	//Checks and gets the channel we want
	std::string name = model->GetName();
	//LOG("%s", name.c_str());
	ResourceAnimation::AnimationChannel* newChannel = mCurrentAnimation->GetChannel(name);

	if (newChannel != nullptr)
	{

		ResourceAnimation::AnimationChannel* channel = mCurrentAnimation->GetChannels().find(model->GetName())->second;
		if (channel == nullptr)
		{
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
				//mCurrentTime = std::fmod(mCurrentTime, mEndTime - mStartTime) + mStartTime;
				mCurrentTime = mStartTime + (mEndTime - mCurrentTime);
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
			CalculateIndexAndLambda(channel, "Translation", mCurrentTime, keyIndex, lambda);

			model->SetPosition(Interpolate(channel->positions[keyIndex - 1], channel->positions[keyIndex], lambda));
		}
		if (channel->hasRotation)
		{
			CalculateIndexAndLambda(channel, "Rotation", mCurrentTime, keyIndex, lambda);

			model->SetRotation(Interpolate(channel->rotations[keyIndex - 1], channel->rotations[keyIndex], lambda));
		}
		//else if (name == "scale") {
		//}
		else { return; }

		model->RecalculateMatrices();
	}
	/*
	for (const auto& child : model->GetChildren())
	{
		GetTransform(child);
	}
	*/
}

void AnimationController::GetTransform_Blending(GameObject* model)
{
	float weight = mCurrentTransitionTime / mTransitionDuration;
	if (weight < 1)
	{
		std::string name = model->GetName();
		ResourceAnimation::AnimationChannel* newChannel = mCurrentAnimation->GetChannel(name);
		ResourceAnimation::AnimationChannel* newNextChannel = mNextAnimation->GetChannel(name);

		if (newChannel != nullptr && newNextChannel != nullptr)
		{

			ResourceAnimation::AnimationChannel* channel = mCurrentAnimation->GetChannels().find(model->GetName())->second;
			ResourceAnimation::AnimationChannel* nextChannel = mNextAnimation->GetChannels().find(model->GetName())->second;
			if (channel == nullptr || nextChannel == nullptr)
			{
				return;
			}

			static float lambda;
			static int keyIndex;

			static float newClipIndex;

			if (channel->hasTranslation)
			{
				CalculateIndexAndLambda(channel, "Translation", mStartTransitionTime, keyIndex, lambda);

				std::vector<float> posTimeStampsVector(channel->posTimeStamps.get(), channel->posTimeStamps.get() + channel->numPositions);
				auto upperBoundIterator = std::upper_bound(posTimeStampsVector.begin(), posTimeStampsVector.end(), mClipStartTime);

				if (upperBoundIterator != posTimeStampsVector.end())
				{
					newClipIndex = std::distance(posTimeStampsVector.begin(), upperBoundIterator);
				}
				else
				{
					newClipIndex = channel->numPositions - 1;
				}

				model->SetPosition(Interpolate(Interpolate(channel->positions[keyIndex - 1], channel->positions[keyIndex], lambda), nextChannel->positions[newClipIndex], weight));
			}
			if (channel->hasRotation)
			{
				CalculateIndexAndLambda(channel, "Rotation", mStartTransitionTime, keyIndex, lambda);

				std::vector<float> rotTimeStampsVector(channel->rotTimeStamps.get(), channel->rotTimeStamps.get() + channel->numRotations);
				auto upperBoundIterator = std::upper_bound(rotTimeStampsVector.begin(), rotTimeStampsVector.end(), mClipStartTime);

				if (upperBoundIterator != rotTimeStampsVector.end())
				{
					newClipIndex = std::distance(rotTimeStampsVector.begin(), upperBoundIterator);
				}
				else
				{
					newClipIndex = channel->numPositions - 1;
				}

				model->SetRotation(Interpolate(Interpolate(channel->rotations[keyIndex - 1], channel->rotations[keyIndex], lambda), nextChannel->rotations[newClipIndex], weight));
			}
			//else if (name == "scale") {
			//}
			else { return; }

			model->RecalculateMatrices();
		}
		/*
		for (const auto& child : model->GetChildren())
		{
			GetTransform_Blending(child);
		}
		*/
	}
	else 
	{
		mTransition = false;
		mCurrentTime = mClipStartTime;
		mCurrentTransitionTime = 0.0f;

		//Change the animations once the transition is done
		mCurrentAnimation = mNextAnimation;
		mNextAnimation = nullptr;
	}
}

void AnimationController::CalculateIndexAndLambda(ResourceAnimation::AnimationChannel* channel, const char* channelType, float timeToFind, int& keyIndex, float& lambda)
{
	if (channelType == "Translation")
	{
		std::vector<float> posTimeStampsVector(channel->posTimeStamps.get(), channel->posTimeStamps.get() + channel->numPositions);
		auto upperBoundIterator = std::upper_bound(posTimeStampsVector.begin(), posTimeStampsVector.end(), timeToFind);

		if (upperBoundIterator != posTimeStampsVector.end())
		{
			keyIndex = std::distance(posTimeStampsVector.begin(), upperBoundIterator);
			lambda = (timeToFind - channel->posTimeStamps[keyIndex - 1]) / (channel->posTimeStamps[keyIndex] - channel->posTimeStamps[keyIndex - 1]);
		}
		else
		{
			keyIndex = channel->numPositions - 1;
			lambda = 1;
		}
	}
	else if (channelType == "Rotation")
	{
		std::vector<float> rotTimeStampsVector(channel->rotTimeStamps.get(), channel->rotTimeStamps.get() + channel->numRotations);
		auto upperBoundIterator = std::upper_bound(rotTimeStampsVector.begin(), rotTimeStampsVector.end(), timeToFind);

		if (upperBoundIterator != rotTimeStampsVector.end())
		{
			keyIndex = std::distance(rotTimeStampsVector.begin(), upperBoundIterator);
			lambda = (timeToFind - channel->rotTimeStamps[keyIndex - 1]) / (channel->rotTimeStamps[keyIndex] - channel->rotTimeStamps[keyIndex - 1]);
		}
		else
		{
			keyIndex = channel->numRotations - 1;
			lambda = 1;
		}
	}
}