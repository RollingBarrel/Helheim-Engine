#include "AnimationState.h"
#include "AnimationTransition.h"
#include "ResourceAnimation.h"


AnimationState::AnimationState(ResourceAnimation* animation)
{
    mAnimation = animation;
    mStartTime = 0.0;
    mEndTime = animation->GetDuration();
}