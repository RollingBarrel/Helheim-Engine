#pragma once
#include "Weapon.h"

class TrailComponent;
class BoxColliderComponent;
class GameObject;
class PlayerController;
struct CollisionData;

class MeleeWeapon : public Weapon
{
public:
	MeleeWeapon(BoxColliderComponent* collider, TrailComponent* trail);
	~MeleeWeapon();

	float GetAttackDuration() override;
	void IncreaseComboStep();

	void Enter() override;
	void Attack(float time) override;
	void Exit() override;

	void OnCollisionEnter(CollisionData* collisionData);

protected:
	//Methods 
	virtual void HitEffect(GameObject* enemy) = 0;  

	//Common
	BoxColliderComponent* mCollider = nullptr;
	TrailComponent* mTrail = nullptr;

	int mComboStep = 0;
	int mNextComboStep = 0;

	float mCombo1st = 1.0f;
	float mCombo2nd = 1.0f;
	float mComboEnd = 1.0f;
	float mHitTime = 0.9f;
	float mMoveSpeed = 0.0f;
	bool mColliderAtivated = false;
	bool mHasHitted = false;

	float mLastComboStartTime = 0.0f;
	bool mMovingForward = false;
	float totalMoveDistance = 5.0f;

	GameObject* mPlayerGO = nullptr;
	PlayerController* mPlayerController = nullptr;

	//Specific 
	float mRange = 1.0f;
	float mDamage = 1;
	int mEnergyCost = 0;
	float mCooldownMultiplier = 1.0f;



};

