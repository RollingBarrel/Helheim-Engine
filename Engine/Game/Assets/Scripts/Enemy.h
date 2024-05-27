#pragma once
#include "Script.h"
#include "Macros.h"
#include "GameObject.h"
class NavMeshController;
class AnimationComponent;

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

		virtual void SetAttracted(bool attracted) { mBeAttracted = attracted; };
		
	protected:
		bool Delay(float delay);
		bool IsPlayerInRange(float range);		
		void DropShield();
		
		int mShieldDropRate = 20;
		float mHealth = 0.0f;
		float mMaxHealth = 6.0f;
		float mSpeed = 1.0f;
		float mRotationSpeed = 1.0f;
		float mActivationRange = 15.0f;
		bool mBeAttracted = false;
		GameObject* mPlayer = nullptr;
		AnimationComponent* mAnimationComponent = nullptr;

	private:
		void ActivateEnemy();

		NavMeshController* mNavMeshControl = nullptr;		
};