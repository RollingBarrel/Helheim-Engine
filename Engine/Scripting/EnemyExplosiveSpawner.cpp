#include "EnemyExplosiveSpawner.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "GameManager.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "BattleArea.h"
#include "Application.h"
#include "PoolManager.h"

CREATE(EnemyExplosiveSpawner)
{
    CLASS(owner);
    SEPARATOR("STATS");
    END_CREATE;
}

EnemyExplosiveSpawner::EnemyExplosiveSpawner(GameObject* owner) : Enemy(owner)
{
	//TODO: Set the enemy values
    mHealth = 15;
}

void EnemyExplosiveSpawner::Start()
{
    Enemy::Start();

    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    mPoolManager = GameManager::GetInstance()->GetPoolManager();
    mCurrentAreaEnemies = GameManager::GetInstance()->GetActiveBattleArea()->GetCurrentEnemies();

    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyExplosiveSpawner::OnCollisionEnter, this, std::placeholders::_1)));
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
        Die();
    }

    mLastSpawnTime += App->GetDt();
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

    LOG("Enemy Health: %f", mHealth);
}

void EnemyExplosiveSpawner::PushBack()
{
    //Trap can't be pushed back
}

void EnemyExplosiveSpawner::Idle()
{
    //mAnimationComponent->SendTrigger("tIdle", 0.2f);
    if (mLastSpawnTime >= mSpawnRate && *mCurrentAreaEnemies < mMaxActiveEnemies)
    {
		mCurrentState = EnemyState::SPAWNING;
	}
}

void EnemyExplosiveSpawner::Spawning()
{
    LOG("Current Enemies: %i", *mCurrentAreaEnemies);
    GameObject* enemy = mPoolManager->Spawn(PoolType::EXPLOSIVE_BALL);
    if (enemy)
    {
        LOG("Enemy spawned");
        ScriptComponent* script = reinterpret_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT));
        Enemy* enemyScript = reinterpret_cast<Enemy*>(script->GetScriptInstance());
        math::float3 pos = mGameObject->GetWorldPosition() + math::float3(0, 0, -4);
        enemy->SetWorldPosition(pos);
        enemy->SetEnabled(true);
        //enemyScript->Reset();
        enemyScript->Init();
        mLastSpawnTime = 0.0f;
    }
    
    mCurrentState = EnemyState::IDLE;
    //mAnimationComponent->SendTrigger("tSpawning", 0.2f);
    //Check if there's room to spawn
}

void EnemyExplosiveSpawner::Die()
{
    //mAnimationComponent->SendTrigger("tDeath", 0.2f);
    if (Delay(0.5f))
    {
        Enemy::Death();
    }
}

void EnemyExplosiveSpawner::OnCollisionEnter(CollisionData* collisionData)
{
}
