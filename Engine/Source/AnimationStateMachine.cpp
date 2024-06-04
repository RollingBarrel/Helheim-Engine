#include "AnimationStateMachine.h"
#include "ResourceAnimation.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResource.h"
#include "AnimationController.h"



AnimationStateMachine::AnimationStateMachine(const std::vector<unsigned int>& animationUids)
{
	for (const auto& resourceAnimation : animationUids)
	{
		mClips.push_back(AnimationClip(resourceAnimation));
	}
	mStates.push_back(AnimationState(std::string("Default")));
}

AnimationStateMachine::~AnimationStateMachine()
{
}

void AnimationStateMachine::AddClip(unsigned int animationUID)
{
	mClips.push_back(AnimationClip(animationUID));
}

void AnimationStateMachine::RemoveClip(int index)
{
	assert(!(index >= mClips.size()));
	mClips.erase(mClips.begin()+index);
}

void AnimationStateMachine::SetClipName(int index, const std::string& name)
{
	assert(!(index >= mClips.size()));
	mClips[index].mName = name;
}

int AnimationStateMachine::GetClipIndex(const std::string& clipName) const
{
	int index = -1;
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

unsigned int AnimationStateMachine::GetClipResource(int index) const
{
	assert(!(index >= mClips.size()));
	return mClips[index].mAnimationUID;
}

void AnimationStateMachine::AddState(const std::string& clipName)
{
	int resource_idx = GetClipResource(GetClipIndex(clipName));
	AnimationState newState = AnimationState(clipName);
	mStates.push_back(newState);
}

void AnimationStateMachine::RemoveState(int index)
{
	assert(!(index >= mStates.size()));
	mStates.erase(mStates.begin() + index);
}

int AnimationStateMachine::GetStateIndex(const std::string& stateName) const
{
	int index = -1;

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
	assert(!(index >= mStates.size()));
	return mStates[index].mClip;
}

const std::string& AnimationStateMachine::GetStateName(int index) const
{
	assert(!(index >= mStates.size()));
	return mStates[index].mName;
}

float AnimationStateMachine::GetStateStartTime(int index) const
{
	assert(!(index >= mStates.size()));
	return mStates[index].mStartTime;
}

float AnimationStateMachine::GetStateEndTime(int index) const
{
	assert(!(index >= mStates.size()));
	return mStates[index].mEndTime;
}

void AnimationStateMachine::SetStateClip(int index, const std::string& clipName)
{
	assert(!(index >= mStates.size()));
	mStates[index].mClip = clipName;
}

void AnimationStateMachine::SetStateName(int index, const std::string& stateName)
{
	assert(!(index >= mStates.size()));
	mStates[index].mName = stateName;
}

void AnimationStateMachine::SetStateStartTime(int index, float time)
{
	assert(!(index >= mStates.size()));
	mStates[index].mStartTime = time;
}

void AnimationStateMachine::SetStateEndTime(int index, float time)
{
	assert(!(index >= mStates.size()));
	mStates[index].mEndTime = time;
}

bool AnimationStateMachine::GetStateLoop(int index) const
{
	assert(!(index >= mStates.size()));
	return mStates[index].mLoop;
}

void AnimationStateMachine::SetStateLoop(int index, bool loop)
{
	assert(!(index >= mStates.size()));
	mStates[index].mLoop = loop;
}

void AnimationStateMachine::DeleteState(int index)
{
	assert(!(index >= mStates.size()));
	std::string state = mStates[index].mName;
	for (int i = 0; i<mTransitions.size(); ++i)
	{
		if (mTransitions[i].mSource == state || mTransitions[i].mTarget == state)
		{
			DeleteTransition(i);
		}
	}

	mStates.erase(mStates.begin() + index);
}

void AnimationStateMachine::AddTransition(const std::string& sourceName, const std::string& targetName, const std::string& trigger)
{
	AnimationTransition newTransition = AnimationTransition(sourceName, targetName, trigger);
	mTransitions.push_back(newTransition);
}

void AnimationStateMachine::RemoveTransition(int index)
{
	assert(!(index >= mTransitions.size()));
	mTransitions.erase(mTransitions.begin() + index);
}

int AnimationStateMachine::GetTransitionIndex(const std::string& sourceName, const std::string& trigger) const
{
	int index = -1;

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
	assert(!(index >= mTransitions.size()));
	return mTransitions[index].mTrigger;
}

const std::string& AnimationStateMachine::GetTransitionSource(int index) const
{
	assert(!(index >= mTransitions.size()));
	return mTransitions[index].mSource;
}

const std::string& AnimationStateMachine::GetTransitionTarget(int index) const
{
	assert(!(index >= mTransitions.size()));
	return mTransitions[index].mTarget;
}

void AnimationStateMachine::SetTransitionTrigger(int index, const std::string& trigger)
{
	assert(!(index >= mTransitions.size()));
	mTransitions[index].mTrigger = trigger;
}

void AnimationStateMachine::DeleteTransition(int index)
{
	assert(!(index >= mTransitions.size()));
	mTransitions.erase(mTransitions.begin() + index);
}

