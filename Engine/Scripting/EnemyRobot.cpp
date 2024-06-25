#include "EnemyRobot.h"
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
#include "AudioManager.h"

CREATE(EnemyRobot){
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::INT, mShieldDropRate);
    MEMBER(MemberType::FLOAT, mChaseDelay);

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


void EnemyRobot::Update()
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

void EnemyRobot::Start()
{
    Enemy::Start();

    mAiAgentComponent = reinterpret_cast<AIAgentComponent*>(mGameObject->GetComponent(ComponentType::AIAGENT));

    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
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

    }
}


void EnemyRobot::Idle()
{
    

    if (IsPlayerInRange(mActivationRange))
    {
        mCurrentState = EnemyState::CHASE;
        mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
        mAnimationComponent->SendTrigger("tWalkForward", 0.2f);
    }
}

void EnemyRobot::Chase()
{
    PlayStepAudio();
    float range = 0.0f;
    if (IsPlayerInRange(mActivationRange))
    {
        if (mAiAgentComponent)
        {
            if(Delay(mChaseDelay)) 
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
        mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
        mAnimationComponent->SendTrigger("tWalkForward", 0.3f);
        mTimerDisengage = 0.0f;
    }
    else if (!playerInRange) 
    {
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
        PlayMeleeAudio();
        MeshRendererComponent* enemyMesh = (MeshRendererComponent*)mPlayer->GetComponent(ComponentType::MESHRENDERER);
        float3 playerPosition = mPlayer->GetWorldPosition();
        float distanceToEnemy = (playerPosition - mGameObject->GetWorldPosition()).Length();
        float3 enemyFrontNormalized = mGameObject->GetFront().Normalized();
        float3 playerToEnemy = (mGameObject->GetWorldPosition() - playerPosition).Normalized();
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
    else 
    {
        mTimerAttack += App->GetDt();
    }
}

void EnemyRobot::RangeAttack() 
{
    std::multiset<Hit> hits;
    Ray ray;
    ray.pos = mGameObject->GetWorldPosition();
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

void EnemyRobot::PlayStepAudio()
{
    // TODO: play sound according the animation
    mStepTimer += App->GetDt();
    if (mStepTimer >= mStepCooldown)
    {
       mStepTimer = 0;
       GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_FOOTSTEP, mGameObject->GetWorldPosition());
    }
}

void EnemyRobot::PlayMeleeAudio()
{
    const char* parameterName = "Speed";
    GameManager::GetInstance()->GetAudio()->PlayOneShot(
        SFX::MEELEE,
        mGameObject->GetWorldPosition(),
        { { parameterName, 0.0f } }
    );
}
