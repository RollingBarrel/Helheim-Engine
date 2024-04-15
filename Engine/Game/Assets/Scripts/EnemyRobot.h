#pragma once
#pragma once
#include <EnemyBase.h>

#include "Target.h"
#include "Geometry/Ray.h"

#include <ScriptComponent.h>


GENERATE_BODY(EnemyRobot);
class EnemyRobot : public EnemyBase
{
	FRIEND(EnemyRobot)

public:
	EnemyRobot(GameObject* owner);
	~EnemyRobot() {}
	void Start() override;
	void Update() override;
	void SetEnemyDamage(int damage) override;


private:
	enum class EnemyState {
		Deploy,
		Forward,
		Backward,
		Left,
		Right,
		RangeAttack,
		MeleeAttack,
		Death
	};

	void ChangeState(EnemyState newState);
	void StateMachine();
	void SearchPlayer() override;
	void MeleeAttack() ;
	void RangeAttack();
	void Shoot();
	void ShootLogic(int damage);
	void Death();

	//*****************************************************
	//FOR TEST UNTIL AI WILL BE AVAILABLE
	void Test_Forward() override;
	void Test_Backward() override;
	void Test_Left() override;
	void Test_Right() override;
	//*****************************************************

	EnemyState mCurrentState;
	EnemyState mPreviousState;
	float mRangetoAttack = 5.0f;
	int mChargeTime = 0.5f;
	int mMeeleDamage = 10;
	int mRangeDamage = 10;

	bool mIsMoving = false;
	bool mIsReadyToShoot = false;
};

