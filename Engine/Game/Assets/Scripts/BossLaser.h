#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"

class GameObject;

GENERATE_BODY(BossLaser);

enum class LaserState
{
    IDLE,
    CHARGING,
    FIRING,
    COOLDOWN
};

class BossLaser : public Script
{
    FRIEND(BossLaser)

public:
    BossLaser(GameObject* owner);
    ~BossLaser() {}

    void Start() override;
    void Update() override;
    void Init(float damage, float duration, float distance, float speed);

private:
    void Charge();
    void Fire();
    void Cooldown();

    LaserState mCurrentState = LaserState::IDLE;

    float mDamage = 0.0f;
    float mLaserDistance = 0.0f;
    float mLaserSpeed = 0.0f;
    float mStateTime = 0.0f;
    float mChargeTime = 1.0f;
    float mLaserDuration = 0.0f;
    float mCooldownDuration = 2.0f;

    GameObject* mLaserEyeBall = nullptr;  // Only one laser now
};
