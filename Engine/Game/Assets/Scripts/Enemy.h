#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

class GameObject;
class AnimationComponent;
class AIAgentComponent;
class Component;

enum class EnemyState
{
	IDLE,
	CHASE,
	CHARGE,
	ATTACK,
};

class Enemy : public Script
{
public:
	Enemy(GameObject* owner);
	~Enemy() {}
	void Start() override;
	void Update() override;

	virtual void TakeDamage(float damage);
	virtual void Death();
	virtual void PushBack();
	virtual void Init();

	bool IsChasing();
	
	//Grenade
	virtual void SetAttracted(bool attracted) { mBeAttracted = attracted; };

protected:
	virtual void Idle();
	virtual void Chase();
	virtual void Charge();
	virtual void Attack();

	virtual void PlayStepAudio() {};
	virtual void PlayAttackAudio() {};

	bool IsPlayerInRange(float range);
	void DropItem();

	//Stats
	float mHealth = 10.0f;
	float mMaxHealth = 6.0f;
	float mSpeed = 1.0f;
	float mRotationSpeed = 1.0f;
	float mChaseDistance = 250.0f;
	float mAttackDistance = 2.0f;
	float mAttackDamage = 2.0f;

	//DropRates
	int mShieldDropRate = 20;
	int mRedEnergyDropRate = 35;
	int mBlueEnergyDropRate = 45;
	bool mBeAttracted = false;

	EnemyState mCurrentState = EnemyState::IDLE;
	GameObject* mPlayer = nullptr;
	AnimationComponent* mAnimationComponent = nullptr;
	AIAgentComponent* mAiAgentComponent = nullptr;

	//Timers
	TimerScript mChargeDurationTimer;
	float mChargeDuration = 0.0f;
	TimerScript mAttackDurationTimer;
	float mAttackDuration = 0.0f;
	TimerScript mAttackCoolDownTimer;
	float mAttackCoolDown = 1.0f;
	TimerScript mDisengageTimer;
	float mDisengageTime = 1.0f;
	TimerScript mDeathTimer;
	float mDeathTime = 1.4f;
	bool mDeath = false;

};