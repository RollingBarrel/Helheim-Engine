#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

GENERATE_BODY(LinearMovement)

class AnimationComponent;
class ParticleSystemComponent;
class GameObject;

class LinearMovement : public Script
{
	FRIEND(LinearMovement)

public:
    LinearMovement(GameObject* owner);
    ~LinearMovement() {}

    void Start() override;
    void Update() override;

    void Movement(float3 target, float speed);

private:

    AnimationComponent* mAnimationComponent = nullptr;
    ParticleSystemComponent* mParticleComponent = nullptr;


    float3 mCurrentPosition = float3::zero;
    float3 mInitialPosition = float3::zero;
    float3 mTargetPosition = float3::zero;
    TimerScript mReturnTimer;
    float mReturnIn;
    float mSpeed = 1.0f;
    bool mTeleportBack = false;
    bool mReachedTarget = false;
    bool mStarted = false;
    bool mLocalPosition = false;
    bool mLookAt = false;
    bool mTargetAGO = false;

    GameObject* mTargetGO = nullptr;

    //Hovering up'n'down movement
    bool mHoveringMovement = true;
    float mHoveringHeight = 0.0f;
    float mHoveringFrequency = 1.0f;
    float3 mHoveringPosition = float3::zero;

    float mHoveringTimer = 0.0f;
};

