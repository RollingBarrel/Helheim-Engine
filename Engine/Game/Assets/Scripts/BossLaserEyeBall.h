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

    void Init(float damage, float distance, float duration, float rotationSpeed, float initialRotation);

private:
    void RotateLaser();

    float mDamage = 3.0f;
    float mDistance = 10.0f;
    float mDuration = 20.0f;
    float mRotationSpeed = 0.5f;
    float mCurrentRotation = 0.0f;
    float mInitialRotation = 0.0f;

    float mElapsedTime = 0.0f;
    GameObject* mLaserOrigin = nullptr;
    GameObject* mLaserTrail = nullptr;
    GameObject* mLaserEnd = nullptr;

    bool mMoveTrail = false;
};
