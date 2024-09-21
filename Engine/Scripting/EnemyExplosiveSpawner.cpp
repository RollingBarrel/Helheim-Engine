#include "EnemyExplosiveSpawner.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "GameManager.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "BattleArea.h"
#include "Application.h"
#include "PoolManager.h"
#include "TimerScript.h"
#include "Application.h"
#include "ModuleScene.h"

CREATE(EnemyExplosiveSpawner)
{
    CLASS(owner);
    MEMBER(MemberType::INT, mEnemiesPerRound);
    MEMBER(MemberType::FLOAT, mSpawnRate);
    MEMBER(MemberType::BOOL, mOnlyOnce);
    END_CREATE;
}

EnemyExplosiveSpawner::EnemyExplosiveSpawner(GameObject* owner) : Enemy(owner) {}

void EnemyExplosiveSpawner::Start()
{
    Enemy::Start();

    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    mPoolManager = GameManager::GetInstance()->GetPoolManager();

    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
        mAnimationComponent->SendTrigger("tIdle", 0.0f);
    }
}

void EnemyExplosiveSpawner::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    if (mSpawnedCounter > 0)
    {
        //Activates animation to open before the enemy spawns to make it appear when the trap is fully open
        if (mSpawnTimer.GetTimePassed() > (mSpawnRate - mGateMoves) && !mIsOpeningTrap)
        {
            mAnimationComponent->SendTrigger("tIdle", 0.0f);
            mAnimationComponent->SendTrigger("tSpawning", 0.0f);
            mIsOpeningTrap = true;
        }

        if (mSpawnTimer.Delay(mSpawnRate))
        {
            GameObject* enemy = mPoolManager->Spawn(PoolType::ROBOT_EXPLOSIVE);
            if (enemy) 
            {
                ScriptComponent* script = reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
                Enemy* enemyScript = reinterpret_cast<Enemy*>(script->GetScriptInstance());
                enemy->SetWorldPosition(mGameObject->GetWorldPosition());
                enemy->SetEnabled(true);
                enemyScript->Init();
                if (mOnlyOnce)
                {
                    mIsActive = false;
                    mSpawnedCounter = 0;
                    return;
                }
                --mSpawnedCounter;
                mIsOpeningTrap = false;
            }
        }
	}
}

bool EnemyExplosiveSpawner::Spawn()
{
    if (mIsActive)
    {
        ++mSpawnedCounter;
        return true;
    }
    return false;
}