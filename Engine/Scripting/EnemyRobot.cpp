#include "EnemyRobot.h"
#include "ModuleScene.h"
#include "Application.h"
#include "PlayerController.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"
#include "Physics.h"
CREATE(EnemyRobot)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mActivationRange);
    MEMBER(MemberType::INT, mHealthDropRate);

    SEPARATOR("RANGE");
    MEMBER(MemberType::FLOAT, mRangeDistance);
    MEMBER(MemberType::FLOAT, mRangeDamage);

    SEPARATOR("MELEE");
    MEMBER(MemberType::FLOAT, mMeleeAttackCoolDown);
    MEMBER(MemberType::FLOAT, mMeleeDistance);
    MEMBER(MemberType::FLOAT, mMeeleDamage);

    END_CREATE;
}

EnemyRobot::EnemyRobot(GameObject* owner) : Enemy(owner)
{
}

void EnemyRobot::Start()
{
    Enemy::Start();
    mAnimationComponent = (AnimationComponent*)mGameObject->GetComponent(ComponentType::ANIMATION);
    if (mAnimationComponent)
    {
        mAnimationComponent->OnStart();
        mAnimationComponent->SetIsPlaying(true);
        mAnimationComponent->SetCurrentClip(0);
    }
}

void EnemyRobot::Update()
{

    switch (mCurrentState) 
    {
    case EnemyState::IDLE:

        Idle();
        if (mAnimationComponent)
        {
            mAnimationComponent->SetCurrentClip(0);
        }

        break;
    case EnemyState::CHASE:

        Chase();
        if (mAnimationComponent)
        {
            mAnimationComponent->SetCurrentClip(1);
        }
        break;
    case EnemyState::ATTACK:
        Attack();
        break;
    }
}

void EnemyRobot::Idle()
{
    if (IsPlayerInRange(mActivationRange))
    {
        mCurrentState = EnemyState::CHASE;
    }
}

void EnemyRobot::Chase()
{

    float range = 0.0f;
    if (IsPlayerInRange(mActivationRange))
    {
        AIAgentComponent* agentComponent = (AIAgentComponent*)mGameObject->GetComponent(ComponentType::AIAGENT);
        if (agentComponent)
        {
            agentComponent->MoveAgent(mPlayer->GetPosition(), mSpeed);
            float3 direction = mPlayer->GetPosition() - agentComponent->GetOwner()->GetPosition();
            direction.y = 0;
            direction.Normalize();
            float angle = std::atan2(direction.x, direction.z);
            mGameObject->SetRotation(float3(0, angle, 0));
        }
        switch (mType)
        {
        case RobotType::RANGE:
            range = mRangeDistance;
            break;
        case RobotType::MELEE:
            range = mMeleeDistance;
            break;
        }

        if (IsPlayerInRange(range))
        {
            mCurrentState = EnemyState::ATTACK;
        }
    }
    else
    {
        mCurrentState = EnemyState::IDLE;
    }
}

void EnemyRobot::Attack()
{
    float range = 0.0f;

    switch (mType)
    {
    case RobotType::RANGE:
        RangeAttack();
        range = mRangeDistance;
        break;
    case RobotType::MELEE:
        MeleeAttack();
        range = mMeleeDistance;
        break;
    }

    if (!IsPlayerInRange(range))
    {
        mCurrentState = EnemyState::CHASE;
    }

}

void EnemyRobot::MeleeAttack() 
{
    if (Delay(mMeleeAttackCoolDown))
    {
       
        MeshRendererComponent* enemyMesh = (MeshRendererComponent*)mPlayer->GetComponent(ComponentType::MESHRENDERER);
        float3 playerPosition = mPlayer->GetPosition();
        float distanceToEnemy = (playerPosition - mGameObject->GetPosition()).Length();
        float3 enemyFrontNormalized = mGameObject->GetFront().Normalized();
        float3 playerToEnemy = (mGameObject->GetPosition() - playerPosition).Normalized();
        float dotProduct = playerToEnemy.Dot(enemyFrontNormalized);

        if (distanceToEnemy < 2.0f && dotProduct < 0)
        {
            PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            if (playerScript != nullptr)
            {
                playerScript->TakeDamage(mMeeleDamage);
            }
        }
    }
}

void EnemyRobot::RangeAttack() 
{
    std::map<float , Hit> hits;
    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.pos.y++;
    ray.dir = mGameObject->GetFront();
    
    float distance = 100.0f;
    hits = Physics::Raycast(&ray);
    
    //Debug::DrawLine(ray.pos, ray.dir * distance, float3(255.0f, 255.0f, 255.0f));
    
        //recorrer todos los hits y hacer da�o a los objetos que tengan tag = target
        for (const std::pair<float, Hit>& hit : hits) 
        {
            if (hit.second.mGameObject->GetTag()->GetName() == "Player") 
            {
                PlayerController* playerScript = (PlayerController*)((ScriptComponent*)hit.second.mGameObject->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
                if (playerScript != nullptr)
                {
                    playerScript->TakeDamage(mRangeDamage);
                }
            }
        }
}