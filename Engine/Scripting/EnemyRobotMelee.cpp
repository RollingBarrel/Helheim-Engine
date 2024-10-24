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
    mChargeDuration = 0.4f;
    mAttackDistance = 1.5f;
    mSwordTrail->SetEnabled(false);
    mDeathAudioPlayed = false;
}

void EnemyRobotMelee::Charge()
{
    mAiAgentComponent->PauseCrowdNavigation();
    mAiAgentComponent->StartCrowdNavigation();
    if (!IsPlayerReachable() && mDisengageTimer.Delay(mDisengageTime))
    {

        mCurrentState = EnemyState::CHASE;
        mSwordTrail->SetEnabled(false);
        mFirstAttack = true;
        return;

    }
    mAnimationComponent->SendTrigger("tAttack", mAttackTransitionDuration);
    mAnimationComponent->RestartStateAnimation();
    if (mFirstAttack &&  mFirstAttackTimer.Delay(mFirstAttackTime))
    {
        mFirstAttack = false;
        mCurrentState = EnemyState::ATTACK;
        mSwordTrail->SetEnabled(true);

    }
    else if (!mFirstAttack && mChargeDurationTimer.Delay(mChargeDuration))
	{
        
 		mCurrentState = EnemyState::ATTACK;
        mSwordTrail->SetEnabled(true);

	}
    mGameObject->LookAt(mPlayer->GetWorldPosition());
}

void EnemyRobotMelee::Attack()
{
    mAnimationComponent->SetIsPlaying(true);
    mAttackTime += App->GetDt();

   static bool attack = true ;
    if ( attack && mAttackTime>=0.2f)
    {
        attack = false;
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
                PlayMeleeAudio();
                playerScript->TakeDamage(mMeeleDamage);
                GameManager::GetInstance()->HitStop();
            }
        }
    }
    if (!attack && mAttackTime >= 0.5f)
    {
        attack = true;
        mCurrentState = EnemyState::CHARGE;
        mAttackTime = 0.0f;
        
    }
}

void EnemyRobotMelee::Death()
{
    Enemy::Death(); 
    if (!mDeathAudioPlayed)
    {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_DEATH, GameManager::GetInstance()->GetPlayerController()->GetPlayerPosition());
        mDeathAudioPlayed = true;
        mSwordTrail->SetEnabled(false);
    }
}

void EnemyRobotMelee::TakeDamage(float damage)
{
    Enemy::TakeDamage(damage);
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_HIT, mGameObject->GetWorldPosition());
}

void EnemyRobotMelee::PlayMeleeAudio()
{
    int randomValue = std::rand() % 2;
    switch (randomValue)
    {
    case 0:
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_SLASH1, mGameObject->GetWorldPosition());

        break;
    case 1:
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_SLASH2, mGameObject->GetWorldPosition());

        break;
    default:
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_ROBOT_SLASH1, mGameObject->GetWorldPosition());

        break;
    }
}