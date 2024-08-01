#include "Enemy.h"
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

    //Hit Effect

	if (!(mGameObject->GetName() == "FinalBoss")) 
		/*
		⠀⠀⠘⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡜⠀⠀⠀
		⠀⠀⠀⠑⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⡔⠁⠀⠀⠀
		⠀⠀⠀⠀⠈⠢⢄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⠴⠊⠀⠀⠀⠀⠀
		⠀⠀⠀⠀⠀⠀⠀⢸⠀⠀⠀⢀⣀⣀⣀⣀⣀⡀⠤⠄⠒⠈⠀⠀⠀⠀⠀⠀⠀⠀
		⠀⠀⠀⠀⠀⠀⠀⠘⣀⠄⠊⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
		⠀
		⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⠛⠛⠛⠋⠉⠈⠉⠉⠉⠉⠛⠻⢿⣿⣿⣿⣿⣿⣿⣿
		⣿⣿⣿⣿⣿⡿⠋⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⢿⣿⣿⣿⣿
		⣿⣿⣿⣿⡏⣀⠀⠀⠀⠀⠀⠀⠀⣀⣤⣤⣤⣄⡀⠀⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿
		⣿⣿⣿⢏⣴⣿⣷⠀⠀⠀⠀⠀⢾⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀⠈⣿⣿
		⣿⣿⣟⣾⣿⡟⠁⠀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣷⢢⠀⠀⠀⠀⠀⠀⠀⢸⣿
		⣿⣿⣿⣿⣟⠀⡴⠄⠀⠀⠀⠀⠀⠀⠙⠻⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⣿
		⣿⣿⣿⠟⠻⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠶⢴⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⣿
		⣿⣁⡀⠀⠀⢰⢠⣦⠀⠀⠀⠀⠀⠀⠀⠀⢀⣼⣿⣿⣿⣿⣿⡄⠀⣴⣶⣿⡄⣿
		⣿⡋⠀⠀⠀⠎⢸⣿⡆⠀⠀⠀⠀⠀⠀⣴⣿⣿⣿⣿⣿⣿⣿⠗⢘⣿⣟⠛⠿⣼
		⣿⣿⠋⢀⡌⢰⣿⡿⢿⡀⠀⠀⠀⠀⠀⠙⠿⣿⣿⣿⣿⣿⡇⠀⢸⣿⣿⣧⢀⣼
		⣿⣿⣷⢻⠄⠘⠛⠋⠛⠃⠀⠀⠀⠀⠀⢿⣧⠈⠉⠙⠛⠋⠀⠀⠀⣿⣿⣿⣿⣿
		⣿⣿⣧⠀⠈⢸⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠟⠀⠀⠀⠀⢀⢃⠀⠀⢸⣿⣿⣿⣿
		⣿⣿⡿⠀⠴⢗⣠⣤⣴⡶⠶⠖⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⡸⠀⣿⣿⣿⣿
		⣿⣿⣿⡀⢠⣾⣿⠏⠀⠠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠛⠉⠀⣿⣿⣿⣿
		⣿⣿⣿⣧⠈⢹⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣰⣿⣿⣿⣿
		⣿⣿⣿⣿⡄⠈⠃⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣴⣾⣿⣿⣿⣿⣿
		⣿⣿⣿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣠⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿
		⣿⣿⣿⣿⣷⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
		⣿⣿⣿⣿⣿⣦⣄⣀⣀⣀⣀⠀⠀⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
		⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿
		⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠙⣿⣿⡟⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿
		⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⠀⠁⠀⠀⠹⣿⠃⠀⣿⣿⣿⣿⣿⣿⣿⣿⣿
		⣿⣿⣿⣿⣿⣿⣿⣿⡿⠛⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⢐⣿⣿⣿⣿⣿⣿⣿⣿⣿
		⣿⣿⣿⣿⠿⠛⠉⠉⠁⠀⢻⣿⡇⠀⠀⠀⠀⠀⠀⢀⠈⣿⣿⡿⠉⠛⠛⠛⠉⠉
		⣿⡿⠋⠁⠀⠀⢀⣀⣠⡴⣸⣿⣇⡄⠀⠀⠀⠀⢀⡿⠄⠙⠛⠀⣀⣠⣤⣤⠄
		*/
	{
		mGameObject->GetComponentsInChildren(ComponentType::MESHRENDERER, mMeshComponents);
		for (unsigned int i = 0; i < mMeshComponents.size(); ++i)
		{
			static_cast<MeshRendererComponent*>(mMeshComponents[i])->CreateUiqueMaterial();
			const ResourceMaterial* material = static_cast<MeshRendererComponent*>(mMeshComponents[i])->GetResourceMaterial();
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
			float angle = std::atan2(direction.x, direction.z);

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
void Enemy::Death()
{
	if (mDeathTimer.Delay(mDeathTime))
	{
		ResetEnemyColor();
		mGameObject->SetEnabled(false);
		DropItem();

		BattleArea* activeBattleArea = GameManager::GetInstance()->GetActiveBattleArea();
		if (activeBattleArea)
		{
			activeBattleArea->EnemyDestroyed();
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
		mAnimationComponent->OnReset();
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
		float3 dropPosition = float3(enemyPosition.x, enemyPosition.y + 0.25f, enemyPosition.z);

		GameObject* itemGameObject = GameManager::GetInstance()->GetPoolManager()->Spawn(poolType);
		itemGameObject->SetWorldPosition(dropPosition);
		ItemDrop* item = static_cast<ItemDrop*>(static_cast<ScriptComponent*>(itemGameObject->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
		item->Init();
	}
}