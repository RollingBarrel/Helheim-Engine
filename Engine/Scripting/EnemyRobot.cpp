#include "EnemyRobot.h"
#include "ModuleScene.h"
#include "Application.h"
#include "PlayerController.h"
#include "AIAGentComponent.h"
#include "AnimationComponent.h"
#include "AnimationStateMachine.h"
#include "Physics.h"
#include "BoxColliderComponent.h"

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

    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));
    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyRobot::OnCollisionEnter, this, std::placeholders::_1)));
    }

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

        mStateMachine->AddTransition(defaultState, sIdle, idleTrigger);

        mStateMachine->AddState(clip, sIdle);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sIdle), float(0.0));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sIdle), float(1.97));

        mStateMachine->AddState(clip, sWalkForward);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sWalkForward), float(2.01));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sWalkForward), float(3.11));

        mStateMachine->AddState(clip, sAttack);
        mStateMachine->SetStateStartTime(mStateMachine->GetStateIndex(sAttack), float(3.15));
        mStateMachine->SetStateEndTime(mStateMachine->GetStateIndex(sAttack), float(10.0));

        mStateMachine->AddTransition(sIdle, sWalkForward, forwardTrigger);
        mStateMachine->AddTransition(sIdle, sAttack, attackTrigger);
       

        mStateMachine->AddTransition(sWalkForward, sIdle, idleTrigger);
        mStateMachine->AddTransition(sWalkForward, sAttack, attackTrigger);
        
        mStateMachine->AddTransition(sAttack, sIdle, idleTrigger);
        mStateMachine->AddTransition(sAttack, sWalkForward, forwardTrigger);

        mAnimationComponent->OnStart();
        mAnimationComponent->SetIsPlaying(true);

        mAnimationComponent->SendTrigger("tIdle", 0.1f);
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
    

    if (IsPlayerInRange(mActivationRange))
    {
        mCurrentState = EnemyState::CHASE;
        mAnimationComponent->SendTrigger("tWalkForward", 0.2f);
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
            mAnimationComponent->SendTrigger("tAttack", 0.2f);
        }
    }
    else
    {
        mCurrentState = EnemyState::IDLE;
        mAnimationComponent->SendTrigger("tIdle", 0.2f);
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

    bool playerInRange = IsPlayerInRange(range);

    if (!playerInRange && mTimerDisengage>1.0f)
    {

        mCurrentState = EnemyState::CHASE;
        mAnimationComponent->SendTrigger("tWalkForward", 0.3f);
        mTimerDisengage = 0.0f;
    }
    else if (!playerInRange) {
        mTimerDisengage += App->GetDt();
    }

}

bool EnemyRobot::IsMoving()
{
    return (mCurrentState == EnemyState::CHASE);
}

void EnemyRobot::MeleeAttack() 
{
    if ( mTimerAttack > mMeleeAttackCoolDown )
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

        mTimerAttack = 0.0f;
    }
    else {
        mTimerAttack += App->GetDt();
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
    Physics::Raycast(ray, hits);
    
    //Debug::DrawLine(ray.pos, ray.dir * distance, float3(255.0f, 255.0f, 255.0f));
    
        //recorrer todos los hits y hacer da�o a los objetos que tengan tag = target
        for (const std::pair<float, Hit>& hit : hits) 
        {
            //if (hit.second.mGameObject->GetTag()->GetName() == "Player") 
            {
                PlayerController* playerScript = (PlayerController*)((ScriptComponent*)hit.second.mGameObject->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
                if (playerScript != nullptr)
                {
                    playerScript->TakeDamage(mRangeDamage);
                }
            }
        }
}

void EnemyRobot::OnCollisionEnter(CollisionData* collisionData)
{
    if (collisionData->collidedWith->GetName().find("Bullet") != std::string::npos)
    {
        TakeDamage(1.0f);
    }
}