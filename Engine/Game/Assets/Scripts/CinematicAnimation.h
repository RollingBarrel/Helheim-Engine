#pragma once
#include "Script.h"
#include "Macros.h"

class AnimationComponent;

GENERATE_BODY(CinematicAnimation);
class CinematicAnimation :public Script
{
    FRIEND(CinematicAnimation)

public:
    CinematicAnimation(GameObject* owner);
    void Awake() override;
    void Start() override;
    void Update() override;

private:
    AnimationComponent* mAnimationComponent = nullptr;
    void PlayAnimation();

    //Transition Times
    float mIdleTransitionDuration = 0.2f;
    float mChaseTransitionDuration = 0.2f;
    float mChargeTransitionDuration = 0.2f;
    float mAttackTransitionDuration = 0.2f;
    float mDeathTransitionDuration = 0.2f;
};