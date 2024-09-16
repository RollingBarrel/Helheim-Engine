#pragma once
#include "Script.h"
#include "Macros.h"
#include "float4.h"
#include "float3.h"
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
	FLEE,
	DEATH,
	//Boss States
	CHARGING_BULLET_HELL,
	CHARGING_LASER,
	WAKE,
	DOWN,
	PHASE
};


class Enemy : public Script
{
public:
	Enemy(GameObject* owner) : Script(owner) {}
	~Enemy() {}
	void Start() override;
	void Update() override;

	virtual void TakeDamage(float damage);
	virtual void Death();
	virtual void PushBack(); 
	virtual void Init();
	
	// DEBUFF
	virtual void Paralyzed(float percentage, bool paralyzed);
	virtual void SetAttracted(bool attracted);

	void ActivateUltVFX();

protected:
	virtual void Idle();
	virtual void Chase();
	virtual void Charge();
	virtual void Attack();
	virtual void Flee();
	virtual void PlayStepAudio();
	virtual void PlayAttackAudio() {};

	bool IsPlayerInRange(float range);
	bool IsPlayerReachable();
	void DropItem();
	void ActivateEnemy();
	void ActivateHitEffect();
	void CheckHitEffect();
	void ResetEnemyColor();
	bool IsDeath() const { return mCurrentState == EnemyState::DEATH; }

	//Stats
	float mHealth = 10.0f;
	float mMaxHealth = 6.0f;
	float mSpeed = 1.0f;
	float mRotationSpeed = 1.0f;
	float mChaseDistance = 250.0f;
	float mAttackDistance = 2.0f;
	float mAttackDamage = 2.0f;

	//DropRates
	int mShieldDropRate = 15;
	int mRedEnergyDropRate = 45;
	int mBlueEnergyDropRate = 80;

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
	TimerScript mHitEffectTimer;
	float mHitEffectTime = 0.15f;
	TimerScript mFleeToAttackTimer;
	float mFleeToAttackTime = 1.0f;


	//Transition Times
	float mIdleTransitionDuration = 0.2f;
	float mChaseTransitionDuration = 0.2f;
	float mChargeTransitionDuration = 0.2f;
	float mAttackTransitionDuration = 0.2f;
	float mDeathTransitionDuration = 0.2f;

	//Movement
	float3 mEnemyCollisionDirection = float3::zero;
	// Step Sound
	TimerScript mStepTimer;
	float mStepDuration = 0.0f;


	//Hit Effect
	bool mHit = false;
	std::vector<Component*> mMeshComponents;
	std::vector <float4> mOgColors;
	GameObject* mUltHitEffectGO = nullptr;
	// DEBUFF
	bool mBeAttracted = false;

	bool mIsParalyzed = false;
	const float mParalyzedDuration = 5.0f;
	TimerScript mParalyzedTimerScript;
	float mParalysisSeverityLevel = 1.0f;



};