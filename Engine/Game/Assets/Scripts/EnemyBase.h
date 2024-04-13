#pragma once
#include <Script.h>
#include "Macros.h"
#include "Math/float3.h"
#include "GameObject.h"
class NavMeshController;
class AnimationComponent;

class EnemyBase : public Script
{
	public:
		EnemyBase(GameObject* owner);
		~EnemyBase() {}
		void Start() override;
		void Update() override;

		virtual void SetEnemyDamage(int damage);

		float mEnemySpeed = 1.0f;
		float mEnemyRotationSpeed = 1.0f;
		GameObject* mAnimationComponentHolder = nullptr;
		GameObject* mOpponent = nullptr;

		int mHealth = 100;
		float mActivationDistance = 15.0f;
		float mAttackDistance = 3.0f;

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

		bool mInAttackDistance = false;

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

		bool mEnemyActivated = false;
		float mTimePassed = 0.0f;
};