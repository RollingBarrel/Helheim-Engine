#include "AnimationTransition.h"
#include "AnimationState.h"

AnimationTransition::AnimationTransition(AnimationState* source, AnimationState* target)
{
	mSource = source;
	mTarget = target;
	//mTrigger = false;
}