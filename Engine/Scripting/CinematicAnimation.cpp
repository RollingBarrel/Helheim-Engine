#include "CinematicAnimation.h"
#include "GameObject.h"
#include "AnimationComponent.h"


CREATE(CinematicAnimation)
{
    CLASS(owner);
    //MEMBER(MemberType::FLOAT, mYawAngle);

    END_CREATE;
}

CinematicAnimation::CinematicAnimation(GameObject* owner) : Script(owner)
{
}

void CinematicAnimation::Awake()
{
}

void CinematicAnimation::Start()
{  
    mAnimationComponent = static_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
    
    }
}

void CinematicAnimation::Update()
{
    PlayAnimation();
}

void CinematicAnimation::PlayAnimation()
{
    //LOG("ANIMATION");
    //mAnimationComponent->OnRestart();

    if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", mIdleTransitionDuration);
    //if (mAnimationComponent) mAnimationComponent->SendTrigger("tChase", mChaseTransitionDuration);
    //if (mAnimationComponent) mAnimationComponent->SendTrigger("tCharge", mChargeTransitionDuration);
    //if (mAnimationComponent) mAnimationComponent->SendTrigger("tAttack", mAttackTransitionDuration);
    //if (mAnimationComponent) mAnimationComponent->SendTrigger("tDeath", mDeathTransitionDuration);
}