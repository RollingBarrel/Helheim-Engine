#include "AnimationTransition.h"
#include "AnimationState.h"

AnimationTransition::AnimationTransition(std::string sourceName, std::string targetName, std::string trigger)
{
	mSource = sourceName;
	mTarget = targetName;
	mTrigger = trigger;
}