#include "EnemyRobot.h"
#include "ModuleScene.h"
#include "Application.h"
#include "PlayerController.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"
#include "AnimationStateMachine.h"
#include "Physics.h"
CREATE(EnemyRobot)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::FLOAT, mActivationRange);
    MEMBER(MemberType::INT, mShieldDropRate);

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
        mStateMachine = mAnimationComponent->GetStateMachine();

    }
    if (mStateMachine)
    {
        std::string clip = "Character";

        std::string defaultState = "default";
        std::string sIdle = "Idle";
        std::string sWalkForward = "Walk Forward";
        std::string sAttack = "Attack";
       

        std::string idleTrigger = "tIdle";
        std::string forwardTrigger = "tWalkForward";
        std::string attackTrigger = "tAttack";
    

        mStateMachine->SetClipName(0, clip);

        mStateMachine->AddState(clip, sIdle);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sIdle), float(6.2));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sIdle), float(11.57));

        mStateMachine->AddState(clip, sWalkForward);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sWalkForward), float(2.89));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sWalkForward), float(3.87));

        mStateMachine->AddState(clip, sAttack);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sAttack), float(3.90));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sAttack), float(4.88));

        mStateMachine->AddTransition(sIdle, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sIdle, sAttack, attackTrigger);
       

        mStateMachine->AddTransition(sWalkForward, sIdle, idleTrigger);
        mStateMachine->AddTransition(sWalkForward, sAttack, attackTrigger);
        
        mStateMachine->AddTransition(sAttack, sIdle, idleTrigger);
        mStateMachine->AddTransition(sAttack, sWalkForward, forwardTrigger);
    }
}

void EnemyRobot::Update()
{
    if (!mBeAttracted) {
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

    Enemy::Update();
}

void EnemyRobot::Idle()
{
    mAnimationComponent->SendTrigger("tIdle", 0.2);

    if (IsPlayerInRange(mActivationRange))
    {
        mCurrentState = EnemyState::CHASE;
    }
}

void EnemyRobot::Chase()
{
    mAnimationComponent->SendTrigger("tWalkForward", 0.2);

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
       mAnimationComponent->SendTrigger("tAttack", 0.2);

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