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
    void Init(float damage, float distance);

private:
    void Charge();
    void Fire();
    void Cooldown();
    void SpawnEyeBalls();

    void ReturnEyeBallsToPool();

    LaserState mCurrentState = LaserState::IDLE;

    float mDamage = 3.0f;
    float mDistance = 5.0f;

    float mCooldownDuration = 2.0f;
    float mChargeTime = 1.0f;
    float mLaserDuration = 25.0f;

    float mEyeRotationSpeed = 2.0f;

    float mStateTime = 0.0f;

    GameObject* mEyeBallPrefab = nullptr;
    std::vector<GameObject*> mEyeBalls;
    std::vector<GameObject*> mEyeBallPool;
    size_t mPoolSize = 4; 
};