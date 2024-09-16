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
        if (mSpawnTimer.Delay(mSpawnRate))
        {
            mAnimationComponent->SendTrigger("tIdle", 0.0f);
            mAnimationComponent->SendTrigger("tSpawning", 0.0f);
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

//void EnemyExplosiveSpawner::Idle()
//{
//    mAnimationComponent->SendTrigger("tIdle", 0.0f);
//	mActiveBattleArea = GameManager::GetInstance()->GetActiveBattleArea();
//
//    if (mActiveBattleArea)
//    {
//        int totalEnemies = mActiveBattleArea->GetCurrentEnemies() + mActiveBattleArea->GetCurrentExplosiveEnemies();
//		if (mSpawnTimer.DelayWithoutReset(mSpawnRate - 2) && totalEnemies < mMaxActiveEnemies) //Spawn rate - 2 to give time to the trap to open and close
//        {
//            mAnimationComponent->SendTrigger("tSpawning", 2.0f);
//            mCurrentState = EnemyState::SPAWNING;
//        }
//    }
//}

//void EnemyExplosiveSpawner::Spawn()
//{
//    //Wait until the trap opens to spawn the ball
//    if (mOpeningTrap.DelayWithoutReset(mGateMoves))
//    {
//        if (!mIsSpawning)
//        {
//            GameObject* enemy = mPoolManager->Spawn(PoolType::ROBOT_EXPLOSIVE);
//            if (enemy)
//            {
//                ScriptComponent* script = reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
//                Enemy* enemyScript = reinterpret_cast<Enemy*>(script->GetScriptInstance());
//                math::float3 pos = mGameObject->GetWorldPosition();
//                enemy->SetWorldPosition(pos);
//                enemy->SetEnabled(true);
//                enemyScript->Init();
//                mActiveBattleArea->AddExplosiveEnemy();
//				mIsSpawning = true;
//            }
//        }
//        // Wait until the trap closes
//        if (mClosingTrap.Delay(mGateMoves*2))
//        {
//            LOG("CLOSING TRAP");
//            mCurrentState = EnemyState::IDLE;
//            mIsSpawning = false;
//			mOpeningTrap.Reset();
//			mSpawnTimer.Reset();
//        }
//	}
//}
