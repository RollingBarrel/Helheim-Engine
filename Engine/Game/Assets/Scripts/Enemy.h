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
		
	protected:
		bool Delay(float delay);
		bool IsPlayerInRange(float range);		
		void DropHealth();
		
		int mHealthDropRate = 20;
		float mHealth = 0.0f;
		float mMaxHealth = 100.0f;
		float mSpeed = 1.0f;
		float mRotationSpeed = 1.0f;
		float mActivationRange = 15.0f;
		GameObject* mPlayer = nullptr;
		AnimationComponent* mAnimationComponent = nullptr;

	private:
		void ActivateEnemy();

		NavMeshController* mNavMeshControl = nullptr;		
};