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

EnemyExplosive::EnemyExplosive(GameObject* owner) : Enemy(owner)
{
    mHealth = 15;

}

void EnemyExplosive::Start()
{
    Enemy::Start();
    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyExplosive::OnCollisionEnter, this, std::placeholders::_1)));
    }
    if (mExplosionWarningGO)
    {
        mWarningSize = mExplosionWarningGO->GetScale();
        mExplosionWarningGO->SetLocalPosition(float3(0.0f,0.1f,0.0f));
    }
    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
    }
}

void EnemyExplosive::Update()
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
        case EnemyState::CHASE:
            Chase();
            break;
        case EnemyState::CHARGING:
            Charging();
            break;
        case EnemyState::EXPLOSION:
            Explosion();
            break;
        case EnemyState::DEATH:
            Die();
        }
    }

    mBeAttracted = false;
}

void EnemyExplosive::Idle()
{
    mAnimationComponent->SendTrigger("tIdle",0.2f);
   mCurrentState = EnemyState::CHASE;
}

void EnemyExplosive::Chase()
{
        mAnimationComponent->SendTrigger("tMovement", 0.2f);
        AIAgentComponent* agentComponent = (AIAgentComponent*)mGameObject->GetComponent(ComponentType::AIAGENT);
        if (agentComponent)
        {
            agentComponent->MoveAgent(mPlayer->GetPosition(), mSpeed);
            float3 direction = mPlayer->GetPosition() - mGameObject->GetPosition();
            direction.y = 0;
            direction.Normalize();
            float angle = std::atan2(direction.x, direction.z);
            mGameObject->SetRotation(float3(0, angle, 0));
        }

        if (IsPlayerInRange(mChargingDistance))
        {
            mCurrentState = EnemyState::CHARGING;
        }
}

void EnemyExplosive::TakeDamage(float damage)
{
}

void EnemyExplosive::Charging()
{
    mAnimationComponent->SendTrigger("tCharging", 0.2f);
    LOG("CHARGING")
       
        if(mWarningTimer>= mExplosionDelay)
        {
            LOG("CHANGE TO EXPLOSION")
                mWarningTimer = 0.0f;
                mCurrentState = EnemyState::EXPLOSION;
        }
    ChargeWarningArea();
}

void EnemyExplosive::Explosion()
{
    mAnimationComponent->SendTrigger("tExplosion", 0.2f);

    LOG("BOOM");
    mExplosionWarningGO->SetScale(float3(0.1f));
    if (IsPlayerInRange(mExplosionDistance))
    {
        PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
        if (playerScript != nullptr)
        {
            playerScript->TakeDamage(mExplosionDamage);
        }
    }

    mCurrentState = EnemyState::DEATH;
        

}

void EnemyExplosive::ChargeWarningArea()
{
    mWarningTimer += App->GetDt();
    float increment = (mExplosionDistance - mExplosionWarningGO->GetScale().x) * App->GetDt();
    float3 newWarningSize = float3(mWarningSize.x += increment, mWarningSize.y, mWarningSize.z += increment);

    mExplosionWarningGO->SetScale(newWarningSize);
    LOG("WarningTimer: %f", mWarningTimer);
    
}
void EnemyExplosive::Die()
{
    mAnimationComponent->SendTrigger("tDeath", 0.2f);
    if (Delay(5.0f))
    {
        Death();
    }
 
}

void EnemyExplosive::OnCollisionEnter(CollisionData* collisionData)
{
}



