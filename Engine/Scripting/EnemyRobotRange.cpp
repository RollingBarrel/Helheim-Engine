#include "EnemyRobotRange.h"
#include "ModuleScene.h"
#include "Application.h"
#include "PlayerController.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"
#include "Physics.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "GameManager.h"
#include "PoolManager.h"
#include "Bullet.h"
#include "TrailComponent.h"

CREATE(EnemyRobotRange) {
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mChaseDelay);
    MEMBER(MemberType::FLOAT, mRangeDistance);
    MEMBER(MemberType::FLOAT, mRangeDamage);
    MEMBER(MemberType::FLOAT, mBulletSpeed);
    MEMBER(MemberType::FLOAT, mTimerAttack);
    MEMBER(MemberType::GAMEOBJECT, mBulletOrigin);

    END_CREATE;
}

EnemyRobotRange::EnemyRobotRange(GameObject* owner) : Enemy(owner)
{
}

void EnemyRobotRange::Start()
{
    Enemy::Start();

    mAiAgentComponent = reinterpret_cast<AIAgentComponent*>(mGameObject->GetComponent(ComponentType::AIAGENT));

    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);

    }
    mAttackCD = mTimerAttack;
}

void EnemyRobotRange::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    Enemy::Update();

    if (!mBeAttracted)
    {
        switch (mCurrentState)
        {
        case EnemyState::IDLE:
            mAnimationComponent->SendTrigger("tIdle", 0.2f);
            Idle();

            break;
        case EnemyState::CHASE:
            
            Chase();
            break;
        case EnemyState::ATTACK:
            
            Attack();
            break;
        }
    }

    mBeAttracted = false;
}

void EnemyRobotRange::Idle()
{


    if (IsPlayerInRange(mActivationRange))
    {
        mCurrentState = EnemyState::CHASE;
        mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
    }
}

void EnemyRobotRange::Chase()
{
    if (IsPlayerInRange(mActivationRange))
    {
        if (mAiAgentComponent)
        {
            if (IsPlayerInRange(mRangeDistance))
            {
                mCurrentState = EnemyState::ATTACK;       
            }
            if (Delay(mChaseDelay))
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
            mAnimationComponent->SendTrigger("tChase", 0.2f);
            mAiAgentComponent->MoveAgent(mSpeed);
            mBulletOrigin->SetWorldPosition(mGameObject->GetWorldPosition());

        }

    }
    else
    {
          mCurrentState = EnemyState::IDLE;
    }
}

void EnemyRobotRange::Attack()
{
    float3 direction = (mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition());
    direction.y = 0;
    direction.Normalize();
    float angle = std::atan2(direction.x, direction.z);

    if (mGameObject->GetWorldRotation().y != angle)
    {
        mGameObject->SetWorldRotation(float3(0, angle, 0));

    }
    if (mAttackCD >= mTimerAttack) 
    {
        mAnimationComponent->SendTrigger("tAttack", 0.2f);
        RangeAttack();
        mAttackCD = 0.0f;
    }
    else
    {
        mAttackCD += App->GetDt();
    }
   
    bool playerInRange = IsPlayerInRange(mRangeDistance);

    if (!playerInRange && mTimerDisengage > 1.0f)
    {
        mCurrentState = EnemyState::CHASE;
        mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
        mTimerDisengage = 0.0f;
    }
    else if (!playerInRange)
    {
        mTimerDisengage += App->GetDt();
    }

}

bool EnemyRobotRange::IsMoving()
{
    return (mCurrentState == EnemyState::CHASE);
}

void EnemyRobotRange::RangeAttack()
{
    float3 bulletOriginPosition = mBulletOrigin->GetWorldPosition();
    GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMYBULLET);
    bulletGO->SetWorldPosition(bulletOriginPosition);
    bulletGO->SetWorldRotation(mGameObject->GetWorldRotation());
    Bullet* bulletScript=reinterpret_cast<Bullet*>(reinterpret_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    ColorGradient gradient;
    gradient.AddColorGradientMark(0.1f, float4(1.0f, 0.0f, 0.0f, 0.0f));
    bulletScript->Init(bulletOriginPosition, mGameObject->GetFront(),mBulletSpeed,1.0f, &gradient,mRangeDamage);
}

void EnemyRobotRange::Death()
{
    mAnimationComponent->SendTrigger("tDeath", 0.3f);
    if (mDeathTimer.Delay(1.4f))
    {
        Enemy::Death();
    }
}

void EnemyRobotRange::Reset()
{
    Enemy::Reset();
    mAnimationComponent->OnReset();
    mAnimationComponent->SendTrigger("tIdle", 0.0f);
}
