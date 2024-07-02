#pragma once
#include "Script.h"
#include "Macros.h"

class GameObject;
class AnimationComponent;
class AIAgentComponent;
class Component;

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

		void AddFootStepAudio(GameObject* audio);

		// DEBUFF
		virtual void GetParalyzed(float percentage);
		virtual void GetParalysisCured(float percentage);

		virtual void SetAttracted(bool attracted) { mBeAttracted = attracted; };

	protected:
		bool Delay(float delay);
		bool IsPlayerInRange(float range);		
		void DropItem();
		
		int mShieldDropRate = 20;
		int mRedEnergyDropRate = 35;
		int mBlueEnergyDropRate = 45;
		float mHealth = 10.0f;
		float mMaxHealth = 6.0f;
		float mSpeed = 1.0f;
		float mRotationSpeed = 1.0f;
		float mActivationRange = 250.0f;
		float mChaseDelay = 1.25f;

		GameObject* mPlayer = nullptr;
		AnimationComponent* mAnimationComponent = nullptr;
		AIAgentComponent* mAiAgentComponent = nullptr;

		// DEBUFF
		bool mBeAttracted = false;

		bool mIsParalyzed = false;
		const float mParalyzedTimer = 5.0f;
		float mCurrentParalyzedTimer = mParalyzedTimer;
		float mParalysisSeverityLevel = 1.0f;

	private:
		void ActivateEnemy();

		GameObject* mFootstepAudioHolder = nullptr;
		
		//Hit Effect
		bool mHit = false;
		bool mDeath = false;
		float mTimePassed = 0.0f;
		std::vector<Component*> mMeshComponents;
		std::vector<unsigned int> mMaterialIds;
};