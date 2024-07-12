#include "EnemyRobotMelee.h"
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

CREATE(EnemyRobotMelee) 
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::INT, mShieldDropRate);
    MEMBER(MemberType::FLOAT, mChaseDelay);
    MEMBER(MemberType::FLOAT, mMeleeAttackCoolDown);
    MEMBER(MemberType::FLOAT, mMeleeDistance);
    MEMBER(MemberType::FLOAT, mMeeleDamage);

    END_CREATE;
}

EnemyRobotMelee::EnemyRobotMelee(GameObject* owner) : Enemy(owner)
{
}

void EnemyRobotMelee::Start()
{
    Enemy::Start();

    mAiAgentComponent = reinterpret_cast<AIAgentComponent*>(mGameObject->GetComponent(ComponentType::AIAGENT));

    mAnimationComponent = reinterpret_cast<AnimationComponent*>(mGameObject->GetComponent(ComponentType::ANIMATION));
    if (mAnimationComponent)
    {
        mAnimationComponent->SetIsPlaying(true);

    }
    mCollider = reinterpret_cast<BoxColliderComponent*>(mGameObject->GetComponent(ComponentType::BOXCOLLIDER));

    if (mCollider)
    {
        mCollider->AddCollisionEventHandler(CollisionEventType::ON_COLLISION_ENTER, new std::function<void(CollisionData*)>(std::bind(&EnemyRobotMelee::OnCollisionEnter, this, std::placeholders::_1)));
    }
}

void EnemyRobotMelee::Update()
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
            mAnimationComponent->SendTrigger("tChase", 0.2f);
            Chase();
            break;
        case EnemyState::ATTACK:
            mAnimationComponent->SendTrigger("tAttack", 0.2f);
            Attack();
            break;
        }
    }

    mBeAttracted = false;
}

void EnemyRobotMelee::Idle()
{


    if (IsPlayerInRange(mActivationRange))
    {
        mCurrentState = EnemyState::CHASE;
        mAiAgentComponent->SetNavigationPath(mPlayer->GetWorldPosition());
    }
}

void EnemyRobotMelee::Chase()
{
    PlayStepAudio();
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

                if (mGameObject->GetWorldRotation().y != angle)
                {
                    mGameObject->SetWorldRotation(float3(0, angle, 0));

                }

            }

        }
        if (IsPlayerInRange(mMeleeDistance))
        {
            mCurrentState = EnemyState::ATTACK;
        }
    }
    else
    {
        mCurrentState = EnemyState::IDLE;
    }
}

void EnemyRobotMelee::Attack()
{
    MeleeAttack();

    bool playerInRange = IsPlayerInRange(mMeleeDistance);

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

bool EnemyRobotMelee::IsMoving()
{
    return (mCurrentState == EnemyState::CHASE);
}

void EnemyRobotMelee::MeleeAttack()
{
    if (mTimerAttack > mMeleeAttackCoolDown)
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
void EnemyRobotMelee::Death() 
{
    mAnimationComponent->SendTrigger("tDeath", 0.3f);
    if(mDeathTimer.Delay(1.4f))
    {
        Enemy::Death();
    }
    if (mAiAgentComponent)
    {
        mAiAgentComponent->PauseCrowdNavigation();
    }
}
void EnemyRobotMelee::Init()
{
    Enemy::Init();
    if (mAnimationComponent)
    {
        mAnimationComponent->OnReset();
        mAnimationComponent->SendTrigger("tIdle",0.0f);
    }
    if (mAiAgentComponent)
    {
        mAiAgentComponent->StartCrowdNavigation();
    }
}
void EnemyRobotMelee::OnCollisionEnter(CollisionData* collisionData)
{
}

void EnemyRobotMelee::PlayStepAudio()
{
    // TODO: play sound according the animation
    mStepTimer += App->GetDt();
    if (mStepTimer >= mStepCooldown)
    {
        mStepTimer = 0;
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_FOOTSTEP, mGameObject->GetWorldPosition());
    }
}

void EnemyRobotMelee::PlayMeleeAudio()
{
    const char* parameterName = "Speed";
    GameManager::GetInstance()->GetAudio()->PlayOneShot(
        SFX::MEELEE,
        mGameObject->GetWorldPosition(),
        { { parameterName, 0.0f } }
    );
}