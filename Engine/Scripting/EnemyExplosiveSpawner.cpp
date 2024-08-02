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

CREATE(EnemyExplosiveSpawner)
{
    CLASS(owner);
    MEMBER(MemberType::INT, mHealth);
    MEMBER(MemberType::FLOAT, mSpawnRate);
    MEMBER(MemberType::INT, mMaxActiveEnemies);
    END_CREATE;
}

EnemyExplosiveSpawner::EnemyExplosiveSpawner(GameObject* owner) : Enemy(owner)
{
}

void EnemyExplosiveSpawner::Start()
{
    Enemy::Start();

    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    mPoolManager = GameManager::GetInstance()->GetPoolManager();
    mCurrentAreaEnemies = GameManager::GetInstance()->GetActiveBattleArea()->GetCurrentEnemies();

    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyExplosiveSpawner::OnCollisionEnter, this, std::placeholders::_1)));
    }
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
    }
}

void EnemyExplosiveSpawner::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    Enemy::Update();

    switch (mCurrentState)
    {
    case EnemyState::IDLE:
        Idle();
        break;
    case EnemyState::SPAWNING:
        Spawning();
        break;
    case EnemyState::DEATH:
        Death();
    }
}

void EnemyExplosiveSpawner::TakeDamage(float damage)
{
    if (mHealth > 0)
    {
        mHealth -= damage;

        if (mHealth <= 0)
        {
            mCurrentState = EnemyState::DEATH;
        }
    }
}

void EnemyExplosiveSpawner::PushBack()
{
    //Trap can't be pushed back
}

void EnemyExplosiveSpawner::Idle()
{
    mAnimationComponent->SendTrigger("tIdle", 0.0f);
    if (mSpawnTimer.Delay(mSpawnRate) && *mCurrentAreaEnemies < mMaxActiveEnemies && mGameObject->IsEnabled())
    {
		mCurrentState = EnemyState::SPAWNING;
	}
}

void EnemyExplosiveSpawner::Spawning()
{
    if (mTrapState == SpawnState::IDLE)
    {
        mAnimationComponent->SendTrigger("tSpawning", 0.0f);
        mTrapState = SpawnState::OPENING;
    }

    if (mAnimationTimer.Delay(1.f) && mTrapState == SpawnState::OPENING)
    {
        GameObject* enemy = mPoolManager->Spawn(PoolType::ROBOT_EXPLOSIVE);
        if (enemy)
        {
            ScriptComponent* script = reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
            Enemy* enemyScript = reinterpret_cast<Enemy*>(script->GetScriptInstance());
            math::float3 pos = mGameObject->GetWorldPosition();
            enemy->SetWorldPosition(pos);
            enemy->SetEnabled(true);
            enemyScript->Init();
        }
        mTrapState = SpawnState::CLOSING;
	}
    // Wait until the trap closes
    if (mAnimationTimer.Delay(1.f) && mTrapState == SpawnState::CLOSING)
    {
        mTrapState = SpawnState::IDLE;
        mCurrentState = EnemyState::IDLE;
    }
}

void EnemyExplosiveSpawner::OnCollisionEnter(CollisionData* collisionData)
{
}
