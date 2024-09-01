#pragma once
#include <Script.h>
#include "Macros.h"
#include "TimerScript.h"

class GameObject;

GENERATE_BODY(BossLaserEyeBall);

class BossLaserEyeBall : public Script
{
    FRIEND(BossLaserEyeBall)

public:

    BossLaserEyeBall(GameObject* owner);

    void Start() override;
    void Update() override;

    void Init(float damage, float duration,float distance,float speed, float rotation);

private:

    void RotateLaser();
    void ActivateLaserVFX();
    void DisableLaserVFX();
    void UpdateLaser();


    float mAimTime = 0.0f;
    TimerScript mAimTimer;

    float mDamage = 0.0f;
    float mInitRotation = 0.0f;
    float mDistance = 0.0f;
    float mDuration = 0.0f;
    float mRotationSpeed = 0.0f;

    float mCurrentRotation = 0.0f;
    float mInitialRotation = 0.0f;

    float mElapsedTime = 0.0f;

    GameObject* mLaserOrigin = nullptr;
    GameObject* mLaserCharge = nullptr;
    GameObject* mLaserEnd = nullptr;

};
