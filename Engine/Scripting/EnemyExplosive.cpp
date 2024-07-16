#include "EnemyExplosive.h"
#include "GameManager.h"
#include "ModuleScene.h"
#include "Application.h"
#include "PlayerController.h"
#include "AnimationComponent.h"
#include "AIAGentComponent.h"
#include "Physics.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ScriptComponent.h"

CREATE(EnemyExplosive)
{
	CLASS(owner);
	SEPARATOR("STATS");
	MEMBER(MemberType::INT, mHealth);
	MEMBER(MemberType::FLOAT, mSpeed);;
	MEMBER(MemberType::FLOAT, mChargingDistance);
	MEMBER(MemberType::FLOAT, mExplosionDistance);
	MEMBER(MemberType::FLOAT, mExplosionDamage);
	MEMBER(MemberType::GAMEOBJECT, mExplosionWarningGO);
	END_CREATE;
}

void EnemyExplosive::Start()
{
	Enemy::Start();

	if (mExplosionWarningGO)
	{
		mWarningSize = mExplosionWarningGO->GetWorldScale();
		mExplosionWarningGO->SetLocalPosition(float3(0.0f, 0.1f, 0.0f));
		mExplosionWarningGO->SetEnabled(false);
	}
}

void EnemyExplosive::Update()
{
	if (GameManager::GetInstance()->IsPaused()) return;

	if (mDeath)
	{
		Death();
	}

	if (!mBeAttracted)
	{
		switch (mCurrentState)
		{
		case ExplosiveEnemyState::IDLE:
			Idle();
			break;
		case ExplosiveEnemyState::CHASE:
			Chase();
			break;
		case ExplosiveEnemyState::CHARGING:
			Charging();
			break;
		case ExplosiveEnemyState::EXPLOSION:
			Explosion();
			break;
		case ExplosiveEnemyState::DEATH:
			Death();
		}
	}

	mBeAttracted = false;
}

void EnemyExplosive::Chase()
{
	mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
	if (mAnimationComponent) mAnimationComponent->SendTrigger("tMovement", 0.2f);

	float3 direction = mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition();
	direction.y = 0;
	direction.Normalize();
	float angle = std::atan2(direction.x, direction.z);
	mGameObject->SetWorldRotation(float3(0, angle, 0));

	if (IsPlayerInRange(mChargingDistance))
	{
		mCurrentState = ExplosiveEnemyState::CHARGING;
		mExplosionWarningGO->SetEnabled(true);
	}
}

void EnemyExplosive::Charging()
{
	if (mAnimationComponent) mAnimationComponent->SendTrigger("tCharging", 0.2f);

	if (mWarningTimer >= mExplosionDelay)
	{
		mWarningTimer = 0.0f;
		mCurrentState = ExplosiveEnemyState::EXPLOSION;
	}
	ChargeWarningArea();
}

void EnemyExplosive::Explosion()
{
	LOG("BOOM");
	mExplosionWarningGO->SetWorldScale(float3(0.1f));
	if (IsPlayerInRange(mExplosionDistance))
	{
		PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
		if (playerScript != nullptr)
		{
			playerScript->TakeDamage(mExplosionDamage);
		}
	}
	mCurrentState = ExplosiveEnemyState::DEATH;
}

void EnemyExplosive::ChargeWarningArea()
{
	mWarningTimer += App->GetDt();
	float increment = (mExplosionDistance - mExplosionWarningGO->GetWorldScale().x) * App->GetDt();
	float3 newWarningSize = float3(mWarningSize.x += increment, mWarningSize.y, mWarningSize.z += increment);

	mExplosionWarningGO->SetWorldScale(newWarningSize);
	LOG("WarningTimer: %f", mWarningTimer);
}



