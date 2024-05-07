#include "AnimationStateMachine.h"
#include "AnimationState.h"
#include "AnimationClip.h"
#include "ResourceAnimation.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleResource.h"
#include "AnimationController.h"



AnimationStateMachine::AnimationStateMachine(std::vector<unsigned int> animationUids)
{

	for (const auto& resourceAnimation : animationUids)
	{
		AnimationClip* newClip = new AnimationClip(resourceAnimation);
		mClips.push_back(newClip);
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
}

void AnimationStateMachine::RemoveClip()
{
}

int AnimationStateMachine::GetClipIndex(std::string clipName)
{
	return 0;
}

unsigned int AnimationStateMachine::GetClipResource(int index)
{
	return mClips[index]->mAnimationUID;
}

void AnimationStateMachine::AddState(std::string clipName, std::string name)
{
}

void AnimationStateMachine::RemoveState()
{
}

int AnimationStateMachine::GetStateIndex(std::string stateName)
{
	return 0;
}

std::string AnimationStateMachine::GetStateClip(int index)
{
	return std::string();
}

void AnimationStateMachine::AddTransition(std::string sourceName, std::string targetName, std::string trigger)
{
}

void AnimationStateMachine::RemoveTransition()
{
}

int AnimationStateMachine::GetTransitionIndex(std::string sourceName, std::string targetName, std::string trigger)
{
	return 0;
}

std::string AnimationStateMachine::GetTransitionTrigger(int index)
{
	return std::string();
}

std::string AnimationStateMachine::GetTransitionSource(int index)
{
	return std::string();
}

std::string AnimationStateMachine::GeTransitionTarget(int index)
{
	return std::string();
}
