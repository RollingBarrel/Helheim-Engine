#include "EnemyExplosiveSpawner.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"
#include "GameManager.h"
#include "AnimationComponent.h"
#include "ScriptComponent.h"
#include "BattleArea.h"

CREATE(EnemyExplosiveSpawner)
{
    CLASS(owner);
    SEPARATOR("STATS");
    END_CREATE;
}

EnemyExplosiveSpawner::EnemyExplosiveSpawner(GameObject* owner) : Enemy(owner)
{
	//TODO: Set the enemy values
    mHealth = 150;
}

void EnemyExplosiveSpawner::Start()
{
    Enemy::Start();

    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));

    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyExplosiveSpawner::OnCollisionEnter, this, std::placeholders::_1)));
    }
}

void EnemyExplosiveSpawner::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    Enemy::Update();
    if (!mBeAttracted)
    {
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
    }

    mBeAttracted = false;
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

void EnemyExplosiveSpawner::Idle()
{
    //mAnimationComponent->SendTrigger("tIdle", 0.2f);
    mCurrentState = EnemyState::SPAWNING;
}

void EnemyExplosiveSpawner::Spawning()
{
    //mAnimationComponent->SendTrigger("tSpawning", 0.2f);
    //Check if there's room to spawn
    //Spawn enemy
}

void EnemyExplosiveSpawner::Die()
{
    //mAnimationComponent->SendTrigger("tDeath", 0.2f);
    if (Delay(0.5f))
    {
        Death();
    }
}

void EnemyExplosiveSpawner::OnCollisionEnter(CollisionData* collisionData)
{
}
