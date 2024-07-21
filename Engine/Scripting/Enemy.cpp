#include "Enemy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "AudioSourceComponent.h"
#include "ScriptComponent.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"

#include "GameManager.h"
#include "PoolManager.h"
#include "ItemDrop.h"
#include "BattleArea.h"

#include "Math/MathFunc.h"


Enemy::Enemy(GameObject* owner) : Script(owner) {}

void Enemy::Start()
{
	ModuleScene* scene = App->GetScene();
	mPlayer = GameManager::GetInstance()->GetPlayer();
	mHealth = mMaxHealth;

	mAiAgentComponent = reinterpret_cast<AIAgentComponent*>(mGameObject->GetComponent(ComponentType::AIAGENT));

	mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
	if (mAnimationComponent)
	{
		mAnimationComponent->SetIsPlaying(true);
	}
}

void Enemy::Update()
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
		case EnemyState::IDLE:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", 0.2f);
			if (mAiAgentComponent) mAiAgentComponent->SetNavigationPath(mGameObject->GetWorldPosition());
			Idle();
			break;
		case EnemyState::CHASE:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tChase", 0.2f);
			Chase();
			break;
		case EnemyState::CHARGE:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tCharge", 0.2f);
			if (mAiAgentComponent) mAiAgentComponent->SetNavigationPath(mGameObject->GetWorldPosition());
			Charge();
			break;
		case EnemyState::ATTACK:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tAttack", 0.2f);
			Attack();
			break;
		}
	}

	mBeAttracted = false;
}

void Enemy::Idle()
{
	if (IsPlayerInRange(mChaseDistance))
	{
		mCurrentState = EnemyState::CHASE;
	}
}

void Enemy::Chase()
{
	PlayStepAudio();
	if (IsPlayerInRange(mChaseDistance))
	{
		if (mAiAgentComponent)
		{
			mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
			float3 direction = (mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition());
			direction.y = 0;
			direction.Normalize();
			float angle = std::atan2(direction.x, direction.z);;

			if (mGameObject->GetWorldRotation().y != angle)
			{
				mGameObject->SetWorldRotation(float3(0, angle, 0));
			}
		}
		if (IsPlayerInRange(mAttackDistance))
		{
			mCurrentState = EnemyState::CHARGE;
		}
	}
	else
	{
		mCurrentState = EnemyState::IDLE;
	}
}

void Enemy::Charge()
{
	if (mChargeDurationTimer.Delay(mChargeDuration))
	{
 		mCurrentState = EnemyState::ATTACK;
	}
}

void Enemy::Attack()
{
	bool playerInRange = IsPlayerInRange(mAttackDistance);
	if (!playerInRange && mDisengageTimer.Delay(mDisengageTime))
	{
		mCurrentState = EnemyState::CHASE;
		mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
	}
	else if (mAttackDurationTimer.Delay(mAttackDuration))
	{
		mCurrentState = EnemyState::CHARGE;
	}
}

bool Enemy::IsPlayerInRange(float range)
{
	float distance = 0.0f;
	distance = (mPlayer) ? mGameObject->GetWorldPosition().Distance(mPlayer->GetWorldPosition()) : inf;

	return (distance <= range);
}

void Enemy::TakeDamage(float damage)
{
	if (mHealth > 0) // TODO: WITHOUT THIS IF DEATH is called two times
	{
		mHealth -= damage;

		if (mHealth <= 0)
		{
			mDeath = true;

			if (mAnimationComponent)
			{
				mAnimationComponent->SendTrigger("tDeath", 0.3f);
			}

			if (mAiAgentComponent)
			{
				mAiAgentComponent->PauseCrowdNavigation();
			}

			BattleArea* activeBattleArea = GameManager::GetInstance()->GetActiveBattleArea();
			if (activeBattleArea)
			{
				activeBattleArea->EnemyDestroyed();
			}

		}
	}
	LOG("Enemy Health: %f", mHealth);
}

void Enemy::Death()
{
	if (mDeathTimer.Delay(mDeathTime))
	{
		mGameObject->SetEnabled(false);
		DropItem();
	}
}

bool Enemy::IsChasing()
{
	return (mCurrentState == EnemyState::CHASE);
}

void Enemy::PushBack()
{
	float3 direction = mGameObject->GetWorldPosition() - mPlayer->GetWorldPosition();
	direction.Normalize();
	mGameObject->SetWorldPosition(mGameObject->GetWorldPosition() + direction * 2.0f);
}

void Enemy::Init()
{
	mDeath = false;
	mHealth = mMaxHealth;

	if (mAnimationComponent)
	{
		mAnimationComponent->OnReset();
		mAnimationComponent->SendTrigger("tIdle", 0.0f);
	}
	if (mAiAgentComponent)
	{
		mAiAgentComponent->StartCrowdNavigation();
	}
}

void Enemy::DropItem()
{
	srand(static_cast<unsigned int>(std::time(nullptr)));
	int randomValue = rand() % 100;

	PoolType poolType = PoolType::LAST;

	if (randomValue < mShieldDropRate)
	{
		poolType = PoolType::SHIELD;
	}
	else if (randomValue < mRedEnergyDropRate)
	{
		poolType = PoolType::RED_ENERGY;
	}
	else if (randomValue < mBlueEnergyDropRate)
	{
		poolType = PoolType::BLUE_ENERGY;
	}

	if (poolType != PoolType::LAST)
	{
		float3 enemyPosition = mGameObject->GetWorldPosition();
		float3 dropPosition = float3(enemyPosition.x, 0.25f, enemyPosition.z);

		GameObject* itemGameObject = GameManager::GetInstance()->GetPoolManager()->Spawn(poolType);
		itemGameObject->SetWorldPosition(dropPosition);
		ItemDrop* item = reinterpret_cast<ItemDrop*>(reinterpret_cast<ScriptComponent*>(itemGameObject->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		item->Init();
	}
}