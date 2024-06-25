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
#include "BulletEnemy.h"
CREATE(EnemyRobotRange) {
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mChaseDelay);
    MEMBER(MemberType::FLOAT, mRangeDistance);
    MEMBER(MemberType::FLOAT, mRangeDamage);
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
      /*  mAnimationComponent->SetIsPlaying(true);*/

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
            //mAnimationComponent->SendTrigger("tIdle", 0.2f);
            Idle();

            break;
        case EnemyState::CHASE:
            //mAnimationComponent->SendTrigger("tChase", 0.2f);
            Chase();
            break;
        case EnemyState::ATTACK:
            //mAnimationComponent->SendTrigger("tAttack", 0.2f);
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
            if (Delay(mChaseDelay))
            {
                mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
                float3 direction = (mPlayer->GetWorldPosition() - mGameObject->GetWorldPosition());
                direction.y = 0;
                direction.Normalize();
                float angle = std::atan2(direction.x, direction.z);;

                if (mGameObject->GetRotation().y != angle)
                {
                    mGameObject->SetRotation(float3(0, angle, 0));

                }

            }

            mAiAgentComponent->MoveAgent(mSpeed);
            mBulletOrigin->SetWorldPosition(mGameObject->GetWorldPosition());

        }

        }

        if (IsPlayerInRange(mRangeDistance))
        {
            mCurrentState = EnemyState::ATTACK;       
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
    float angle = std::atan2(direction.x, direction.z);;

    if (mGameObject->GetRotation().y != angle)
    {
        mGameObject->SetRotation(float3(0, angle, 0));

    }
    if (mAttackCD >= mTimerAttack) 
    {

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
    LOG("I NEED MORE BOOLETS")
    float3 bulletOriginPosition = mBulletOrigin->GetWorldPosition();
    GameObject* bulletGO = GameManager::GetInstance()->GetPoolManager()->Spawn(PoolType::ENEMYBULLET);
    bulletGO->SetWorldPosition(bulletOriginPosition);
    bulletGO->SetRotation(mGameObject->GetRotation());
    BulletEnemy* bulletScript=reinterpret_cast<BulletEnemy*>(reinterpret_cast<ScriptComponent*>(bulletGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    bulletScript->Init();


}

void EnemyRobotRange::Death()
{
    //mAnimationComponent->SendTrigger("tDeath", 0.3f);
    if (Delay(0.5f))
    {
        Enemy::Death();
    }

}
