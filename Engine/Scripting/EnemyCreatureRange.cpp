#include "EnemyCreatureRange.h"
#include "Application.h"
#include "GameObject.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "AIAGentComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
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
	MEMBER(MemberType::GAMEOBJECT, mLaserEnd);
	MEMBER(MemberType::GAMEOBJECT, mLaserCharge);
	SEPARATOR("VFX");
	MEMBER(MemberType::GAMEOBJECT, mUltHitEffectGO);
	END_CREATE;
}

void EnemyCreatureRange::Start()
{
	Enemy::Start();
	Init();

	if (mLaserOrigin)	mLaserOrigin->SetEnabled(false);
	if (mLaserEnd) mLaserEnd->SetEnabled(false);
	if (mLaserCharge)
	{
		mLaserCharge->SetEnabled(false);
		if (mLaserOrigin) mLaserCharge->SetLocalPosition(mLaserOrigin->GetLocalPosition());
	}

	mDeathTime = 2.20f;
	mAimTime = mChargeDuration * 0.8f;
	mLaserSound = GameManager::GetInstance()->GetAudio()->Play(SFX::ENEMY_CREATURE_LASER, mLaserSound, mGameObject->GetWorldPosition());
	GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_CREATURE_LASER, mLaserSound,true);
}

void EnemyCreatureRange::Update()
{
	Enemy::Update();

	if (mCurrentState != EnemyState::ATTACK)
	{
		GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_CREATURE_LASER, mLaserSound, true);

		if (mLaserOrigin)	mLaserOrigin->SetEnabled(false);
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

	if (!mAimTimer.DelayWithoutReset(mAimTime))
	{
		mGameObject->LookAt(mPlayer->GetWorldPosition());
	}
	
	if (mLaserCharge)	mLaserCharge->SetEnabled(true);
}

void EnemyCreatureRange::Attack()
{
	GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_CREATURE_LASER, mLaserSound, false);

	Enemy::Attack();
	Rotate();
	mAimTimer.Reset();

	mAnimationComponent->SendTrigger("tAttack", 0.2f);
	
	if (mLaserOrigin)	mLaserOrigin->SetEnabled(true);
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
	}
	else
	{
		float3 originPosition = mLaserOrigin->GetLocalPosition();
		mLaserEnd->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mAttackDistance));
	}
}

void EnemyCreatureRange::Rotate()
{
	float3 direction = (mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition());
	direction.y = 0.0f;
	direction.Normalize();
	
	float3 currentDirection = mGameObject->GetFront();
	currentDirection.y = 0.0f;
	currentDirection.Normalize();
	float currentRadianAngle = std::atan2(currentDirection.x, currentDirection.z);

	float angleDifference = currentDirection.AngleBetween(direction);
	angleDifference = (currentDirection.Cross(direction).y > 0) ? angleDifference : angleDifference * -1;

	float rotationSpeed = mAttackRotationSpeed * App->GetDt();
	float newAngle = currentRadianAngle + Clamp(angleDifference, -rotationSpeed, rotationSpeed);

	mGameObject->SetLocalRotation(float3(0.0f, newAngle, 0.0f));
}


