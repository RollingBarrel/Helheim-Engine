#include "EnemyCreatureRange.h"
#include "Application.h"
#include "GameObject.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "AIAGentComponent.h"

#include "GameManager.h"
#include "PoolManager.h"
#include "PlayerController.h"

#include "ColorGradient.h"
#include "Physics.h"
#include "Geometry/Ray.h"
#include <MathFunc.h>


CREATE(EnemyCreatureRange)
{
	CLASS(owner);
	SEPARATOR("STATS");
	MEMBER(MemberType::FLOAT, mMaxHealth);
	MEMBER(MemberType::FLOAT, mSpeed);
	MEMBER(MemberType::FLOAT, mRotationSpeed);
	MEMBER(MemberType::FLOAT, mChargeDuration);
	MEMBER(MemberType::FLOAT, mAttackDamage);
	MEMBER(MemberType::FLOAT, mAttackDuration);
	MEMBER(MemberType::FLOAT, mAttackRotationSpeed);
	MEMBER(MemberType::FLOAT, mAttackCoolDown);
	SEPARATOR("STATES");
	MEMBER(MemberType::FLOAT, mAttackDistance);
	SEPARATOR("GAME OBJECTS");
	MEMBER(MemberType::GAMEOBJECT, mLaserOrigin);
	MEMBER(MemberType::GAMEOBJECT, mLaserTrail);
	MEMBER(MemberType::GAMEOBJECT, mLaserEnd);
	MEMBER(MemberType::GAMEOBJECT, mLaserCharge);
	END_CREATE;
}

void EnemyCreatureRange::Start()
{
	Enemy::Start();
	Init();

	if (mLaserOrigin)	mLaserOrigin->SetEnabled(false);
	if (mLaserTrail) mLaserTrail->SetEnabled(false);
	if (mLaserEnd) mLaserEnd->SetEnabled(false);
	if (mLaserCharge)
	{
		mLaserCharge->SetEnabled(false);
		if (mLaserOrigin) mLaserCharge->SetLocalPosition(mLaserOrigin->GetLocalPosition());
	}

	mDeathTime = 1.0f;

}

void EnemyCreatureRange::Update()
{
	Enemy::Update();

	if (mCurrentState != EnemyState::ATTACK)
	{
		if (mLaserOrigin)	mLaserOrigin->SetEnabled(false);
		if (mLaserTrail)	mLaserTrail->SetEnabled(false);
		if (mLaserEnd) mLaserEnd->SetEnabled(false);
	}
	if (mCurrentState != EnemyState::CHARGE)
	{
		if (mLaserCharge)	mLaserCharge->SetEnabled(false);
	}
}

void EnemyCreatureRange::Charge()
{
	Enemy::Charge();
	Rotate();

	if (mLaserCharge)	mLaserCharge->SetEnabled(true);
}

void EnemyCreatureRange::Attack()
{
	Enemy::Attack();
	Rotate();

	mAnimationComponent->SendTrigger("tAttack", 0.2f);
	
	if (mLaserOrigin)	mLaserOrigin->SetEnabled(true);
	if (mLaserTrail)	mLaserTrail->SetEnabled(true);
	if (mLaserEnd)		mLaserEnd->SetEnabled(true);
	
	if (mAttackCoolDownTimer.Delay(mAttackCoolDown))
	{
		mDoDamage = true;
	}

	Hit hit;
	Ray ray;
	ray.dir = mGameObject->GetFront();
	ray.pos = mLaserOrigin->GetWorldPosition();

	Physics::Raycast(hit, ray, mAttackDistance);
	if (hit.IsValid())
	{
		if (hit.mGameObject->GetTag().compare("Player") == 0 && mDoDamage)
		{
			ScriptComponent* playerScript = static_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
			PlayerController* player = static_cast<PlayerController*>(playerScript->GetScriptInstance());
			player->TakeDamage(mAttackDamage);
			mDoDamage = false;
		}
		mLaserEnd->SetWorldPosition(hit.mHitPoint);

		//Trails WorkAround
		if (mMoveTrail)
		{
			mLaserTrail->SetWorldPosition(hit.mHitPoint);
			mMoveTrail = false;
		}
		else
		{
			mMoveTrail = true;
			mLaserTrail->SetWorldPosition(mLaserOrigin->GetWorldPosition());
		}
	}
	else
	{
		float3 originPosition = mLaserOrigin->GetLocalPosition();
		mLaserEnd->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mAttackDistance));

		//Trails WorkAround
		if (mMoveTrail)
		{
			mLaserTrail->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mAttackDistance));
			mMoveTrail = false;
		}
		else
		{
			mLaserTrail->SetLocalPosition(originPosition);
			mMoveTrail = true;
		}
	}
}

void EnemyCreatureRange::Rotate() //TODO IMPROVE ROTATE BEHAVIOUR
{
	float3 direction = (mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition());
	direction.y = 0.0f;
	direction.Normalize();

	float targetRadianAngle = std::atan2(direction.x, direction.z);
	float targetEulerAngle = RadToDeg(targetRadianAngle);
	if (targetEulerAngle < 0)
	{
		targetEulerAngle = 360.0f + targetEulerAngle;
	}

	float3 currentDirection = mGameObject->GetFront();
	currentDirection.y = 0.0f;
	currentDirection.Normalize();

	float currentRadianAngle = std::atan2(currentDirection.x, currentDirection.z);
	float currentEulerAngle = RadToDeg(currentRadianAngle);
	if (currentEulerAngle < 0)
	{
		currentEulerAngle = 360.0f + currentEulerAngle;
	}

	//if (abs(targetEulerAngle - currentEulerAngle) > 180.0f)
	//{
	//	float rotation = (mGameObject->GetLocalEulerAngles().y + DegToRad(mAttackRotationSpeed)) * App->GetDt();
	//	mGameObject->SetLocalRotation(float3(0.0f, rotation, 0.0f));
	//}
	//else
	//{
	//	float rotation = -1 * (mGameObject->GetLocalEulerAngles().y + DegToRad(mAttackRotationSpeed)) * App->GetDt();
	//	mGameObject->SetLocalRotation(float3(0.0f, rotation, 0.0f));
	//}



	float attackRotaionSpeed = (targetEulerAngle - currentEulerAngle > 0.0f) ? mAttackRotationSpeed : mAttackRotationSpeed * -1.0f;
	mGameObject->SetLocalRotation(float3(0.0f, currentRadianAngle + DegToRad(attackRotaionSpeed) * App->GetDt(), 0.0f));
}


