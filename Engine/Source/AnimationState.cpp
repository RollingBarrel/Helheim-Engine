#include "AnimationState.h"
#include "AnimationClip.h"
#include "AnimationTransition.h"
#include "ResourceAnimation.h"


AnimationState::AnimationState(std::string clipName, std::string name)
{
    mName = name;
    mClip = clipName;
    mStartTime = 0.0;
}