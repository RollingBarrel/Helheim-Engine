﻿#include "Enemy.h"
#include "Application.h"
#include "ModuleScene.h"
#include "GameObject.h"
#include "AudioSourceComponent.h"
#include "ScriptComponent.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"
#include "BoxColliderComponent.h"
#include "MeshRendererComponent.h"
#include "ResourceMaterial.h"
#include "ModuleDetourNavigation.h"

#include "Physics.h"
#include "Geometry/Ray.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerController.h"
#include "PoolManager.h"
#include "ItemDrop.h"
#include "BattleArea.h"
#include <cmath>
#include <iostream>
#include "Math/MathFunc.h"

void Enemy::Start()
{
	ModuleScene* scene = App->GetScene();
	mPlayer = GameManager::GetInstance()->GetPlayer();
	mHealth = mMaxHealth;

    //Hit Effect

	if (mGameObject->GetName() != "FinalBoss") 
	{
		//Randn step duration to not sound so mess
		mStepDuration = 0.4 + static_cast<float>(rand()) / RAND_MAX * (0.7 - 0.4);
		mGameObject->GetComponentsInChildren(ComponentType::MESHRENDERER, mMeshComponents);
		for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
		{
			const ResourceMaterial* material = static_cast<MeshRendererComponent*>(mMeshComponents[i])->GetResourceMaterial();
			static_cast<MeshRendererComponent*>(mMeshComponents[i])->CreateUniqueMaterial();
			mOgColors.push_back(material->GetBaseColorFactor());
		}
	}
	mAiAgentComponent = static_cast<AIAgentComponent*>(mGameObject->GetComponent(ComponentType::AIAGENT));
	
	mAnimationComponent = static_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
	if (mAnimationComponent)
	{
		mAnimationComponent->SetIsPlaying(true);
	}
}

void Enemy::Update()
{
	if (GameManager::GetInstance()->IsPaused()) return;

	if (mIsParalyzed)
	{
		if (mParalyzedTimerScript.Delay(mParalyzedDuration))
		{
			Paralyzed(mParalysisSeverityLevel, false);
		}
	}
	ActivateEnemy();

    //Hit Effect
    CheckHitEffect();
	mEnemyCollisionDirection = float3::zero;
}

void Enemy::CheckHitEffect()
{
    if (mHit)
    {
        if (mHitEffectTimer.Delay(mHitEffectTime))
        {
			ResetEnemyColor();
            mHit = false;
        }
    }
}

//void Enemy::CheckUltHitVFX()
//{
//	if (mUltHit)
//	{
//		if (mUltEffectTimer.Delay(mUltEffectTime)) {
//			ActivateUltVFX();
//		}
//	}
//}

void Enemy::ResetEnemyColor()
{
	for (size_t i = 0; i < mMeshComponents.size(); i++)
	{
		MeshRendererComponent* meshComponent = static_cast<MeshRendererComponent*>(mMeshComponents[i]);
		meshComponent->SetEnableBaseColorTexture(true);
		meshComponent->SetBaseColorFactor(mOgColors[i]);
	}
}

void Enemy::ActivateEnemy()
{
	if (!mBeAttracted)
	{
		switch (mCurrentState)
		{
		case EnemyState::IDLE:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", mIdleTransitionDuration);
			if (mAiAgentComponent) mAiAgentComponent->SetNavigationPath(mGameObject->GetWorldPosition());
			Idle();
			break;
		case EnemyState::CHASE:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tChase", mChaseTransitionDuration);
			Chase();
			break;
		case EnemyState::FLEE:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tChase", mChaseTransitionDuration);
			Flee();
			break;
		case EnemyState::CHARGE:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tCharge", mChargeTransitionDuration);
			if (mAiAgentComponent) mAiAgentComponent->SetNavigationPath(mGameObject->GetWorldPosition());
			Charge();
			break;
		case EnemyState::ATTACK:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tAttack", mAttackTransitionDuration);
			Attack();
			break;
		case EnemyState::DEATH:
			if (mAnimationComponent) mAnimationComponent->SendTrigger("tDeath", mDeathTransitionDuration);
			Death();
			break;
		}
	}
	else 
	{
		mBeAttracted = false;
		mAiAgentComponent->SetEnable(true);
	}
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

		if (mAiAgentComponent)
		{
			mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
			float3 dir = mAiAgentComponent->GetDirection();
			if (!dir.Equals(float3::zero)) mGameObject->LookAt(mGameObject->GetWorldPosition() + float3(dir.x, 0.0f, dir.z));
		}
		
		if (IsPlayerReachable())
		{
			mCurrentState = EnemyState::CHARGE;
		}

}

void Enemy::Flee()
{
	if (mFleeToAttackTimer.Delay(mFleeToAttackTime))
	{
		mCurrentState = EnemyState::ATTACK;
		return;
	}
	PlayStepAudio();	
		if (mAiAgentComponent)
		{
			float distance = mGameObject->GetWorldPosition().Distance(mPlayer->GetWorldPosition());
			float3 newDir = mGameObject->GetWorldPosition() - mPlayer->GetWorldPosition();
			float collisionDotProduct = newDir.Dot(mEnemyCollisionDirection);
			if (collisionDotProduct < 0.0f)
			{
				newDir = newDir - mEnemyCollisionDirection.Mul(collisionDotProduct);
			}
			float3 newPos = mGameObject->GetWorldPosition() + newDir * mSpeed;
			mAiAgentComponent->SetNavigationPath(App->GetNavigation()->FindNearestPoint(newPos, float3(1.0f)));
			mGameObject->LookAt(mGameObject->GetWorldPosition() + mAiAgentComponent->GetDirection());
		
		}

		if (!IsPlayerInRange(mAttackDistance))
		{
			mCurrentState = EnemyState::IDLE;
		}
}

void Enemy::PlayStepAudio()
{
	if (mStepTimer.Delay(mStepDuration))
	{
		//GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_FOOTSTEP, mGameObject->GetWorldPosition());
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
	if (mAiAgentComponent) mAiAgentComponent->PauseCrowdNavigation();
	bool playerReachable = IsPlayerReachable();
	if (!playerReachable && mDisengageTimer.Delay(mDisengageTime))
	{
		if (mAiAgentComponent) mAiAgentComponent->StartCrowdNavigation();
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

bool Enemy::IsPlayerReachable()
{
	bool reachable = false;

	if (IsPlayerInRange(mAttackDistance))
	{
		Hit hit;
		Ray ray;

		float3 enemyPosition = mGameObject->GetWorldPosition();
		float3 playerPosition = mPlayer->GetWorldPosition();

		ray.pos = enemyPosition;
		ray.dir = (playerPosition - enemyPosition).Normalized();

		float distance = enemyPosition.Distance(playerPosition);

		std::vector<std::string> ignoreTags = { "Bullet", "BattleArea", "Trap", "Drop", "Enemy" };
		Physics::Raycast(hit, ray, distance, &ignoreTags);

		if (hit.IsValid() && hit.mGameObject->GetTag().compare("Player") == 0)
		{
			reachable = true;
		}
	}
	return reachable;
}

void Enemy::TakeDamage(float damage)
{
	if (mHealth > 0) // TODO: WITHOUT THIS IF DEATH is called two times
	{
		ActivateHitEffect();
		mHealth -= damage;

		if (mHealth <= 0)
		{
			mCurrentState = EnemyState::DEATH;

			BoxColliderComponent* collider = static_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
			if (collider) collider->SetEnable(false);

			if (mAiAgentComponent)	mAiAgentComponent->PauseCrowdNavigation();
		}
	}
	LOG("Enemy Health: %f", mHealth);
}

void Enemy::ActivateHitEffect()
{
    if (mHit) return;
   //LOG("HIT EFFECT");
    for (Component* mesh : mMeshComponents)
    {
        MeshRendererComponent* meshComponent = static_cast<MeshRendererComponent*>(mesh);
        meshComponent->SetEnableBaseColorTexture(false);
        meshComponent->SetBaseColorFactor(float4(255.0f, 0.0f, 0.0f, 1.0f));
    }
    mHit = true;
}

void Enemy::ActivateUltVFX()
{
	if (mUltHitEffectGO)
	{
		mUltHitEffectGO->SetEnabled(false); 
		mUltHitEffectGO->SetEnabled(true);
	}
}

void Enemy::Death()
{
	mVanishingTime += App->GetDt();
	if (mVanishingTime >= mDeathTime*0.75)
	{
	for (size_t i = 0; i < mMeshComponents.size(); i++)
	{
		MeshRendererComponent* meshRender = static_cast<MeshRendererComponent*>(mMeshComponents[i]);
		const ResourceMaterial* material = meshRender->GetResourceMaterial();
		float4 baseColor = material->GetBaseColorFactor();
		float4 endColor = material->GetBaseColorFactor();
		endColor.w = 0.0f;
		meshRender->SetBaseColorFactor(baseColor.Lerp(endColor, mVanishingTime /(mDeathTime*10)));
	}
	}
	if (mVanishingTime>=mDeathTime)
	{
		//GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_DEATH, mGameObject->GetWorldPosition());

		BattleArea* activeBattleArea = GameManager::GetInstance()->GetActiveBattleArea();
		if (activeBattleArea)
		{
			activeBattleArea->EnemyDestroyed(mGameObject);
		}
		ResetEnemyColor();
		mVanishingTime = 0.0f;
		mGameObject->SetEnabled(false);
		DropItem();

		if (mAnimationComponent)
		{
			mAnimationComponent->ResetAnimationComponent();
		}
	}
}

void Enemy::PushBack()
{
	float3 direction = mGameObject->GetWorldPosition() - mPlayer->GetWorldPosition();
	direction.Normalize();
	mGameObject->SetWorldPosition(mGameObject->GetWorldPosition() + direction * 2.0f);
}

void Enemy::Init()
{
	mHealth = mMaxHealth;
	mCurrentState = EnemyState::IDLE;

	if (mAnimationComponent)
	{
		mAnimationComponent->ResetAnimationComponent();
		mAnimationComponent->SendTrigger("tIdle", 0.0f);
	}
	if (mAiAgentComponent)
	{
		mAiAgentComponent->StartCrowdNavigation();
	}
}

void Enemy::Paralyzed(float percentage, bool paralyzed)
{
	if (paralyzed)
	{
		mIsParalyzed = true;
		mSpeed *= percentage;
		mParalyzedTimerScript = TimerScript();
		mParalysisSeverityLevel = percentage;
	}
	else 
	{
		mIsParalyzed = false;
		mSpeed /= percentage;

		mParalysisSeverityLevel = 1.0f;
	}
}

void Enemy::SetAttracted(bool attracted)
{ 
	mBeAttracted = attracted;
	// Sometime, AI component is over everything, I need to set it disable to make blackhole works
	mAiAgentComponent->SetEnable(!attracted);
}


void Enemy::DropItem()
{
	int shieldDropRate = mShieldDropRate;
	int redDropRate = mRedEnergyDropRate;
	int blueDropRate = mBlueEnergyDropRate;
	float playerShield= GameManager::GetInstance()->GetPlayerController()->GetCurrentShield();
	if( playerShield<=50.0f)
	{
		shieldDropRate = 50;
		redDropRate = 70;
		blueDropRate = 90;
	}
	srand(static_cast<unsigned int>(std::time(nullptr)));
	int randomValue = rand() % 100;
	PoolType poolType = PoolType::LAST;

	if (randomValue < shieldDropRate)
	{
		poolType = PoolType::SHIELD;
	}
	else if (randomValue < redDropRate)
	{
		poolType = PoolType::RED_ENERGY;
	}
	else if (randomValue < blueDropRate)
	{
		poolType = PoolType::BLUE_ENERGY;
	}

	if (poolType != PoolType::LAST)
	{
		float3 enemyPosition = mGameObject->GetWorldPosition();
		float3 dropPosition = float3(enemyPosition.x, enemyPosition.y + 0.25f, enemyPosition.z);

		GameObject* itemGameObject = GameManager::GetInstance()->GetPoolManager()->Spawn(poolType);
		itemGameObject->SetWorldPosition(dropPosition);
		ItemDrop* item = static_cast<ItemDrop*>(static_cast<ScriptComponent*>(itemGameObject->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		item->Init();
	}
}