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

    mAiAgentComponent = reinterpret_cast<AIAgentComponent*>(mGameObject->GetComponent(ComponentType::AIAGENT));
    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));

    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyExplosive::OnCollisionEnter, this, std::placeholders::_1)));
    }
    if (mExplosionWarningGO)
    {
        mWarningSize = mExplosionWarningGO->GetWorldScale();
        mExplosionWarningGO->SetLocalPosition(float3(0.0f,0.1f,0.0f));
        mExplosionWarningGO->SetEnabled(false);
    }
    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);
    }

    if (mAiAgentComponent)
    {
        mAiAgentComponent->StartCrowdNavigation();
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
        mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
        mAnimationComponent->SendTrigger("tMovement", 0.2f);

           
        float3 direction = mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition();
        direction.y = 0;
        direction.Normalize();
        float angle = std::atan2(direction.x, direction.z);
        mGameObject->SetWorldRotation(float3(0, angle, 0));
        
        
        if (IsPlayerInRange(mChargingDistance))
        {
            mCurrentState = EnemyState::CHARGING;
            mExplosionWarningGO->SetEnabled(true);
        }
}

void EnemyExplosive::TakeDamage(float damage)
{
}

void EnemyExplosive::Charging()
{
    mAnimationComponent->SendTrigger("tCharging", 0.2f);

       
        if(mWarningTimer>= mExplosionDelay)
        {
                mWarningTimer = 0.0f;
                mCurrentState = EnemyState::EXPLOSION;
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

    mCurrentState = EnemyState::DEATH;
        

}

void EnemyExplosive::ChargeWarningArea()
{
    mWarningTimer += App->GetDt();
    float increment = (mExplosionDistance - mExplosionWarningGO->GetWorldScale().x) * App->GetDt();
    float3 newWarningSize = float3(mWarningSize.x += increment, mWarningSize.y, mWarningSize.z += increment);

    mExplosionWarningGO->SetWorldScale(newWarningSize);
    LOG("WarningTimer: %f", mWarningTimer);
    
}
void EnemyExplosive::Die()
{
    mAnimationComponent->SendTrigger("tDeath", 0.2f);
    if (Delay(0.5f))
    {
        Death();
    }
    if (mAiAgentComponent)
    {
        mAiAgentComponent->PauseCrowdNavigation();
    }
 
}

void EnemyExplosive::OnCollisionEnter(CollisionData* collisionData)
{
}



