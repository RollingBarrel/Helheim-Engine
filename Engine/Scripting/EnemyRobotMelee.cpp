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
    MEMBER(MemberType::FLOAT, mAttackCoolDown);
    SEPARATOR("STATES");
    MEMBER(MemberType::FLOAT, mAttackDistance);
    SEPARATOR("VFX");
    MEMBER(MemberType::GAMEOBJECT, mSwordTrail);
    MEMBER(MemberType::GAMEOBJECT, mUltHitEffectGO);
    END_CREATE;
}

void EnemyRobotMelee::Start()
{
    Enemy::Start();
    mDisengageTime = 0.1f;
    mSwordTrail->SetEnabled(false);
    mDeathAudioPlayed = false;
}

void EnemyRobotMelee::Charge()
{
    Enemy::Charge();
    mGameObject->LookAt(mPlayer->GetWorldPosition());
}

void EnemyRobotMelee::Attack()
{
    Enemy::Attack();
    if (mCurrentState == EnemyState::CHARGE) 
    {
        mSwordTrail->SetEnabled(true);
    }    
    if (mCurrentState == EnemyState::CHASE) 
    {
        mSwordTrail->SetEnabled(false);
    }
    if (mAttackCoolDownTimer.Delay(mAttackCoolDown))
    {
        mAnimationComponent->RestartStateAnimation();
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
                GameManager::GetInstance()->HitStop();
            }
        }
    }
}

void EnemyRobotMelee::Death()
{
    Enemy::Death(); 
    if (!mDeathAudioPlayed)
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_DEATH, GameManager::GetInstance()->GetPlayerController()->GetPlayerPosition());
        mDeathAudioPlayed = true;
    }
}

void EnemyRobotMelee::TakeDamage(float damage)
{
    Enemy::TakeDamage(damage);
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_HIT, mGameObject->GetWorldPosition());
}

void EnemyRobotMelee::PlayMeleeAudio()
{
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_SLASH, mGameObject->GetWorldPosition());
}