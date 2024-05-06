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

}

void AnimationStateMachine::Update()
{


	//if (mController)
	//{
	//	// Do AnimationController update
	//	mController->Update(mOwner);

	//}
}

unsigned int AnimationStateMachine::GetClipResource(int index)
{
	return mClips[index]->mAnimationUID;
}