#include "AnimationState.h"
#include "AnimationClip.h"
#include "AnimationTransition.h"
#include "ResourceAnimation.h"


AnimationState::AnimationState(AnimationClip* clip, std::string name)
{
    mName = name;
    mClip = clip->mName;
    mStartTime = 0.0;
}