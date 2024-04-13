#pragma once
#include <EnemyBase.h>

GENERATE_BODY(EnemyExplosive);
class EnemyExplosive : public EnemyBase
{
	FRIEND(EnemyExplosive)
	public:
		EnemyExplosive(GameObject* owner);
		~EnemyExplosive() {}
		void Start() override;
		void Update() override;

	private:
		enum class EnemyState {
			Deploy,
			Forward,
			Backward,
			Left,
			Right,
			Armed,
			Explosion
		};

		void ChangeState(EnemyState newState);
		void StateMachine();
		void SearchPlayer() override;
		void SetEnemyDamage(int damage) override;
		void Armed();
		void Explosion();

		//*****************************************************
		//FOR TEST UNTIL AI WILL BE AVAILABLE
		void Test_Forward() override;
		void Test_Backward() override;
		void Test_Left() override;
		void Test_Right() override;
		//*****************************************************

		EnemyState mCurrentState;	
};