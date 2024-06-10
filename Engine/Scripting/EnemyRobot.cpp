#include "EnemyRobot.h"
#include "ModuleScene.h"
#include "Application.h"
#include "PlayerController.h"
#include "AIAGentComponent.h"
#include "Physics.h"
#include "BoxColliderComponent.h"
#include "GameObject.h"
#include "ScriptComponent.h"

CREATE(EnemyRobot){
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

    
}

void EnemyRobot::Update()
{
    Enemy::Update();

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

    mBeAttracted = false;
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

    bool playerInRange = IsPlayerInRange(range);

    if (!playerInRange && mTimerDisengage>1.0f)
    {

        mCurrentState = EnemyState::CHASE;
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
            if (hit.second.mGameObject->GetTag() == "Player") 
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