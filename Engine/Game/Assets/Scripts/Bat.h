#pragma once
#include "MeleeWeapon.h"
#include "Script.h"
#include "Macros.h"

class TrailComponent;
class BoxColliderComponent;
class GameObject;
struct CollisionData;

enum class WeaponUpgrade {
    NONE,
    RED,
    BLUE
};

class Bat : public MeleeWeapon
{
public:
	Bat(BoxColliderComponent* collider);
	~Bat();

	void Enter() override;
    void Update(float deltaTime);
    void OnCollisionEnter(CollisionData* collisionData);
    void DealDamage(GameObject* enemy);
	void Attack(float time) override;
	void Exit() override;

private:
    void PlayHitSound();
	TrailComponent* mTrail = nullptr;
    BoxColliderComponent* mCollider = nullptr;

    float mEnergyCost = 10.0f;       
    float mCooldownModifier = 1.f;

    float mComboTimer = 0.0f;
    float mComboCurrentTime = 0.0f;
    bool  mIsComboActive = false;
    bool  mIsNextComboStep = false;
    float mBreakCombo = 0.0f;

    // Player Dash
    float  mMoveRange = 15.f;
    float  mMoveDuration = 4.0f;



};
