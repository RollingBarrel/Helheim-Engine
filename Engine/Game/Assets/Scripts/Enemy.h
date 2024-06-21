#pragma once
#include "Script.h"
#include "Macros.h"

class GameObject;
class NavMeshController;
class AnimationComponent;
class AIAgentComponent;

enum class EnemyType : int
{
	ROBOT_MELEE = 0,
	ROBOT_RANGE,
	COUNT
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
		virtual bool IsMoving();
		virtual void Reset();

		virtual void SetAttracted(bool attracted) { mBeAttracted = attracted; };

		void AddFootStepAudio(GameObject* audio);
		
	protected:
		bool Delay(float delay);
		bool IsPlayerInRange(float range);		
		void DropItem();
		
		int mShieldDropRate = 20;
		int mShotgunDropRate = 35;
		int mMachineGunDropRate = 45;
		float mHealth = 0.0f;
		float mMaxHealth = 6.0f;
		float mSpeed = 1.75f;
		float mRotationSpeed = 1.0f;
		float mActivationRange = 250.0f;
		float mChaseDelay = 1.25f;
		bool mBeAttracted = false;
		GameObject* mPlayer = nullptr;
		AnimationComponent* mAnimationComponent = nullptr;
		AIAgentComponent* mAiAgentComponent = nullptr;

	private:
		void ActivateEnemy();

		NavMeshController* mNavMeshControl = nullptr;	
		GameObject* mFootstepAudioHolder = nullptr;
		
};