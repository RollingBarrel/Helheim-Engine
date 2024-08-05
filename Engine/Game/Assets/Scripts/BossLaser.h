#pragma once
#include <Script.h>
#include "Macros.h"
#include "float3.h"

class BoxColliderComponent;
class GameObject;
struct CollisionData;

GENERATE_BODY(BossLaser);

enum class LaserState {
	CHARGING,
	FIRING,
	COOLDOWN
};


class BossLaser : public Script
{
	FRIEND(BossLaser)
public:
	BossLaser(GameObject* owner);
	~BossLaser();

	void Start() override;
	void Update() override;

	void Init(float damage, float range);


private:

    void ChangeState(LaserState newState);
    void UpdateCharging(float dt);
    void UpdateFiring(float dt);
    void UpdateCooldown(float dt);
    void SpawnEyeballs();

    void SummonEyeballs(const std::vector<float3>& position);

    // Helper methods
    void mMoveLaser(float dt);
    void mApplyDamage();

    BoxColliderComponent* mCollider = nullptr;
    float mDamage = 3.0f;
    float mRange = 10.0f;
    float mDuration = 5.0f;
    float mRotationSpeed = 2.0f;
    float mChargeTime = 1.0f;
    float mCooldownTime = 2.0f;
    float mCurrentAngle = 0.0f;
    float mDirection = 1.0f;
    float mStateTimer = 0.0f;
    LaserState mCurrentState = LaserState::CHARGING;

    std::vector<GameObject*> mEnemyEye;

    // Laser VFX components
    GameObject* mLaserOrigin = nullptr;
    GameObject* mLaserTrail = nullptr;
    GameObject* mLaserEnd = nullptr;
    GameObject* mLaserCharge = nullptr;

};
