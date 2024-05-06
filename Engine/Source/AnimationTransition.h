#pragma once
#include <string>

class AnimationState;

class AnimationTransition
{
public:
	AnimationTransition(AnimationState* source, AnimationState* target);
	std::string mTrigger;
	AnimationState* mSource;
	AnimationState* mTarget;

};
