#pragma once
#include <Script.h>
#include "Macros.h"

class GameObject;

GENERATE_BODY(BossLaserEyeBall);

class BossLaserEyeBall : public Script
{
    FRIEND(BossLaserEyeBall)

public:

    BossLaserEyeBall(GameObject* owner);

    void Start() override;
    void Update() override;

    void Init(float distance, float duration);

private:

    void RotateLaser();
    float mDamage = 3.0f;

    float mDistance = 10.0f;
    float mDuration = 5.0f;
    float mRotationSpeed = 1.0f;
    float mCurrentRotation = 0.0f;
    float mInitialRotation = 0.0f;
    float mRotationRange = 45.0f;

    float mElapsedTime = 0.0f;
    bool mIsRotating = false;
    bool mMoveTrail = false;

    GameObject* mLaserOrigin = nullptr;
    GameObject* mLaserTrail = nullptr;
    GameObject* mLaserEnd = nullptr;

};
