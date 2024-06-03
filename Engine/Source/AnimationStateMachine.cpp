#include "AnimationStateMachine.h"
#include "ResourceAnimation.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResource.h"
#include "AnimationController.h"



AnimationStateMachine::AnimationStateMachine(std::vector<unsigned int> animationUids)
{

	for (const auto& resourceAnimation : animationUids)
	{
		mClips.push_back(AnimationClip(resourceAnimation));
	}
	mStates.push_back(AnimationState(std::string("Default")));
}

AnimationStateMachine::~AnimationStateMachine()
{
	mClips.clear();
	mTransitions.clear();
	mStates.clear();
}


void AnimationStateMachine::AddClip(unsigned int animationUID)
{

	mClips.push_back(AnimationClip(animationUID));

}

void AnimationStateMachine::RemoveClip(int index)
{

	mClips.erase(mClips.begin()+index);
}

void AnimationStateMachine::SetClipName(int index, std::string& name)
{
	mClips[index].mName = name;
}

const int AnimationStateMachine::GetClipIndex(const std::string& clipName) const
{
	int index = 0;

	for (size_t i = 0; i < mClips.size(); i++)
	{

		if (clipName == mClips[i].mName)
		{
			index = i;
			break;
		}

	}
	return index;
}

const unsigned int AnimationStateMachine::GetClipResource(int index) const
{
	return mClips[index].mAnimationUID;
}

const AnimationState& AnimationStateMachine::AddState(std::string& clipName)
{
	int resource_idx = GetClipResource(GetClipIndex(clipName));
	AnimationState newState = AnimationState(clipName);
	mStates.push_back(newState);
	return newState;
}

void AnimationStateMachine::RemoveState(int index)
{

	mStates.erase(mStates.begin() + index);

}

const int AnimationStateMachine::GetStateIndex(const std::string& stateName) const
{
	int index = 0;

	for (size_t i = 0; i < mStates.size(); i++)
	{
		if (stateName == mStates[i].mName)
		{
			index = i;
			break;
		}

	}
	return index;
}

const std::string& AnimationStateMachine::GetStateClip(int index) const
{
	return mStates[index].mClip;
}

const std::string& AnimationStateMachine::GetStateName(int index) const
{
	return mStates[index].mName;
}

const float AnimationStateMachine::GetStateStartTime(int index) const
{
	return mStates[index].mStartTime;
}

const float AnimationStateMachine::GetStateEndTime(int index) const
{
	return mStates[index].mEndTime;
}

void AnimationStateMachine::SetStateClip(int index, std::string& clipName)
{
	mStates[index].mClip = clipName;
}

void AnimationStateMachine::SetStateName(int index, std::string& stateName)
{
	mStates[index].mName = stateName;
}

void AnimationStateMachine::SetStateStartTime(int index, float time)
{
	mStates[index].mStartTime = time;
}

void AnimationStateMachine::SetStateEndTime(int index, float time)
{
	mStates[index].mEndTime = time;
}

void AnimationStateMachine::DeleteState(int index)
{
	std::string state = mStates[index].mName;
	int i = 0;
	for (AnimationTransition transition : mTransitions)
	{
		if (transition.mSource == state || transition.mTarget == state)
		{
			DeleteTransition(i);
		}
		i++;
	}

	mStates.erase(mStates.begin() + index);
}

const AnimationTransition& AnimationStateMachine::AddTransition(const std::string& sourceName, const std::string& targetName, const std::string& trigger)
{

	AnimationTransition newTransition = AnimationTransition(sourceName, targetName, trigger);
	mTransitions.push_back(newTransition);
	return newTransition;
}

void AnimationStateMachine::RemoveTransition(int index)
{

	mTransitions.erase(mTransitions.begin() + index);
}

const int AnimationStateMachine::GetTransitionIndex(std::string& sourceName, std::string& trigger) const
{
	int index = 0;

	for (size_t i = 0; i < mTransitions.size(); i++)
	{
		if (sourceName == mTransitions[i].mSource  && sourceName == mTransitions[i].mTrigger) 
		{
			index = i;
			break;

		}
	}
	return index;
}

const std::string& AnimationStateMachine::GetTransitionTrigger(int index) const
{
	return mTransitions[index].mTrigger;
}

const std::string& AnimationStateMachine::GetTransitionSource(int index) const
{
	return mTransitions[index].mSource;
}

const std::string& AnimationStateMachine::GetTransitionTarget(int index) const
{
	return mTransitions[index].mTarget;
}
