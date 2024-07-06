#pragma once
#include "Script.h"
#include "Macros.h"

class GameObject;
class AnimationComponent;
class AIAgentComponent;
class Component;


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
		virtual void Init();

		virtual void SetAttracted(bool attracted) { mBeAttracted = attracted; };

		void AddFootStepAudio(GameObject* audio);
		
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
		bool mBeAttracted = false;
		GameObject* mPlayer = nullptr;
		AnimationComponent* mAnimationComponent = nullptr;
		AIAgentComponent* mAiAgentComponent = nullptr;

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