#include "EnemyRobotMelee.h"
#include "Application.h"
#include "GameObject.h"

#include "ScriptComponent.h"
#include "AnimationComponent.h"
#include "AIAGentComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerController.h"

CREATE(EnemyRobotMelee) 
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mRotationSpeed);
    MEMBER(MemberType::INT, mShieldDropRate);
    MEMBER(MemberType::FLOAT, mMeeleDamage);
    SEPARATOR("STATES");
    MEMBER(MemberType::FLOAT, mAttackDistance);
    END_CREATE;
}

void EnemyRobotMelee::Attack()
{
    Enemy::Attack();
    
    if (mAttackCoolDownTimer.Delay(mAttackCoolDown))
    {
        PlayMeleeAudio();
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
    }
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