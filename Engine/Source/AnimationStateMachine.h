#pragma once
#include <vector>

class AnimationController;
class AnimationState;
class ResourceAnimation;
class GameObject;
class AnimationTransition;
class AnimationClip;

class AnimationStateMachine
{

public:
	AnimationStateMachine(std::vector<unsigned int> animationUids);
	void Update();
	void SetLoop(bool loop);

	unsigned int GetClipResource(int index);


	std::vector<AnimationState*> mStates;
	std::vector<AnimationTransition*> mTransistions;
	std::vector<AnimationClip*> mClips;
	AnimationState* mCurrentState;

	AnimationController* mController;
	//GameObject* mOwner;

private:

	bool mLoop = true;

};
