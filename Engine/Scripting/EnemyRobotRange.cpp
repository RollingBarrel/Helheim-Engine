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

CREATE(EnemyRobotRange) {
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::INT, mShieldDropRate);
    MEMBER(MemberType::FLOAT, mChaseDelay);
    MEMBER(MemberType::FLOAT, mRangeDistance);
    MEMBER(MemberType::FLOAT, mRangeDamage);


    END_CREATE;
}

EnemyRobotRange::EnemyRobotRange(GameObject* owner) : Enemy(owner)
{
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

void EnemyRobotRange::Start()
{
    Enemy::Start();

    mAiAgentComponent = reinterpret_cast<AIAgentComponent*>(mGameObject->GetComponent(ComponentType::AIAGENT));

    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mStateMachine = mAnimationComponent->GetStateMachine();

    }
}


void EnemyRobotRange::Idle()
{


    if (IsPlayerInRange(mActivationRange))
    {
        mCurrentState = EnemyState::CHASE;
        mAiAgentComponent->SetNavigationPath(mPlayer->GetPosition());
        mAnimationComponent->SendTrigger("tWalkForward", 0.2f);
    }
}

void EnemyRobotRange::Chase()
{


    float range = 0.0f;
    if (IsPlayerInRange(mActivationRange))
    {
        if (mAiAgentComponent)
        {
            if (Delay(mChaseDelay))
            {
                mAiAgentComponent->SetNavigationPath(mPlayer->GetPosition());
                float3 direction = (mPlayer->GetPosition() - mGameObject->GetPosition());
                direction.y = 0;
                direction.Normalize();
                float angle = std::atan2(direction.x, direction.z);;

                if (mGameObject->GetRotation().y != angle)
                {
                    mGameObject->SetRotation(float3(0, angle, 0));

                }

            }

            mAiAgentComponent->MoveAgent(mSpeed);

        }
            range = mRangeDistance;

        }

        if (IsPlayerInRange(range))
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
    float range = 0.0f;


        RangeAttack();
        range = mRangeDistance;
        

    bool playerInRange = IsPlayerInRange(range);

    if (!playerInRange && mTimerDisengage > 1.0f)
    {

        mCurrentState = EnemyState::CHASE;
        mAiAgentComponent->SetNavigationPath(mPlayer->GetPosition());
        mAnimationComponent->SendTrigger("tWalkForward", 0.3f);
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
    std::multiset<Hit> hits;
    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.pos.y++;
    ray.dir = mGameObject->GetFront();

    float distance = 100.0f;
    //Physics::Raycast(hits, ray);  THIS IS THE OLD RAYCAST

    //Debug::DrawLine(ray.pos, ray.dir * distance, float3(255.0f, 255.0f, 255.0f));

        //recorrer todos los hits y hacer da�o a los objetos que tengan tag = target
    for (const Hit& hit : hits)
    {
        if (hit.mGameObject->GetTag() == "Player")
        {
            PlayerController* playerScript = (PlayerController*)((ScriptComponent*)hit.mGameObject->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            if (playerScript != nullptr)
            {
                playerScript->TakeDamage(mRangeDamage);
            }
        }
    }
}