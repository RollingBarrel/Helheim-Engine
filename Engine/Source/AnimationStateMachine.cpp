#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "AnimationClip.h"
#include "ResourceAnimation.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResource.h"
#include "AnimationController.h"
#include "AnimationTransition.h"



AnimationStateMachine::AnimationStateMachine(std::vector<unsigned int> animationUids)
{

	for (const auto& resourceAnimation : animationUids)
	{
		mClips.push_back(AnimationClip(resourceAnimation));
	}

	//AnimationState defaultState =  new AnimationState(mClips[0],"default")

}

void AnimationStateMachine::Update()
{


	//if (mController)
	//{
	//	// Do AnimationController update
	//	mController->Update(mOwner);

	//}
}

void AnimationStateMachine::AddClip(unsigned int animationUID)
{

	mClips.push_back(AnimationClip(animationUID));

}

void AnimationStateMachine::RemoveClip(int index)
{

	mClips.erase(mClips.begin()+index);
}

int AnimationStateMachine::GetClipIndex(std::string& clipName)
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

unsigned int AnimationStateMachine::GetClipResource(int index)
{
	return mClips[index].mAnimationUID;
}

void AnimationStateMachine::AddState(std::string& clipName, std::string& name)
{
	mStates.push_back(AnimationState(clipName,name));
}

void AnimationStateMachine::RemoveState(int index)
{

	mStates.erase(mStates.begin() + index);

}

int AnimationStateMachine::GetStateIndex(std::string& stateName)
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

std::string& AnimationStateMachine::GetStateClip(int index)
{
	return mStates[index].mClip;
}

std::string& AnimationStateMachine::GetStateName(int index)
{
	return mStates[index].mName;
}

void AnimationStateMachine::SetStateClip(int index, std::string& clipName)
{
	mStates[index].mClip = clipName;
}

void AnimationStateMachine::SetStateName(int index, std::string& stateName)
{
	mStates[index].mName = stateName;
}

void AnimationStateMachine::AddTransition(std::string& sourceName, std::string& targetName, std::string& trigger)
{
	mTransistions.push_back(AnimationTransition(sourceName, targetName, trigger));
}

void AnimationStateMachine::RemoveTransition(int index)
{

	mTransistions.erase(mTransistions.begin() + index);
}

int AnimationStateMachine::GetTransitionIndex(std::string& sourceName, std::string& targetName, std::string& trigger)
{
	int index = 0;

	for (size_t i = 0; i < mTransistions.size(); i++)
	{
		if (sourceName == mTransistions[i].mSource && targetName == mTransistions[i].mTarget && sourceName == mTransistions[i].mTrigger) 
		{
			index = i;
			break;

		}
	}
	return index;
}

std::string& AnimationStateMachine::GetTransitionTrigger(int index)
{
	return mTransistions[index].mTrigger;
}

std::string& AnimationStateMachine::GetTransitionSource(int index)
{
	return mTransistions[index].mSource;
}

std::string& AnimationStateMachine::GeTransitionTarget(int index)
{
	return mTransistions[index].mTarget;
}
