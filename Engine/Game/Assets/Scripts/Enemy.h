#pragma once
#include <Script.h>
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

		virtual void Hit(float damage);
		
	protected:
		bool OpponentDistance(float enemyDistance);
		bool Delay(float delay);
		virtual void SearchPlayer();
				
		//*****************************************************
		//FOR TEST UNTIL AI WILL BE AVAILABLE
		virtual void Test_Forward();
		virtual void Test_Backward();
		virtual void Test_Left();
		virtual void Test_Right();
		//*****************************************************

		float mHealth = 100.0f;
		float mSpeed = 1.0f;
		float mRotationSpeed = 1.0f;
		float mActivationRange = 15.0f;
		bool mInAttackDistance = false;
		GameObject* mAnimationComponentHolder = nullptr;

	private:
		void ActivateEnemy();
		
		//************************************************
		//FOR TEST UNTIL AI WILL BE AVAILABLE
		void Test_Controls();
		void Move(float3 direction);
		void Test_Mouse_Rotation();
		//************************************************
		
		NavMeshController* mNavMeshControl = nullptr;
		AnimationComponent* mAnimationComponent = nullptr;

		bool mIsActive = false;
		float mTimePassed = 0.0f;
		
		
		
		
		
};