#include "EnemyExplosive.h"
#include "Application.h"
#include "ModuleScene.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "ParticleSystemComponent.h"
#include "BoxColliderComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerCamera.h"

CREATE(EnemyExplosive)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mMaxHealth);
    MEMBER(MemberType::FLOAT, mSpeed);
    MEMBER(MemberType::FLOAT, mAttackDamage);
    MEMBER(MemberType::FLOAT, mChargeDuration);
    MEMBER(MemberType::FLOAT, mExplosionRadius);
    SEPARATOR("STATES");
    MEMBER(MemberType::FLOAT, mAttackDistance);
    SEPARATOR("GAMEOBJECTS");
    MEMBER(MemberType::GAMEOBJECT, mExplosionWarningGO);
    MEMBER(MemberType::GAMEOBJECT, mExplosionParticle);


    END_CREATE;
}

void EnemyExplosive::Start()
{
    Enemy::Start();
    mMaxHealth = 1.0f;
    mAttackDistance = 1.0f;
    mChargeDuration = 1.0f;

    if (mExplosionWarningGO)
    {
        mExplosionWarningGO->SetLocalPosition(float3(0.0f, 0.1f, 0.0f));
        mExplosionWarningGO->SetEnabled(false);
    }

    if (mExplosionParticle)
    {
        mExplosionParticle->SetEnabled(false);
    }

    mChargeSound = GameManager::GetInstance()->GetAudio()->Play(SFX::ENEMY_EXPLOSIVE_PREEXPLOSION);
    mAttackSound = GameManager::GetInstance()->GetAudio()->Play(SFX::ENEMY_EXPLOSIVE_EXPLOSION);
    mStepSound = GameManager::GetInstance()->GetAudio()->Play(SFX::ENEMY_EXPLOSIVE_STEPS);

    mExplosionPlaying = false;
    mChargePlaying = false;

    GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_EXPLOSIVE_PREEXPLOSION, mChargeSound, true);
    GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_EXPLOSIVE_EXPLOSION, mAttackSound, true);
    GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_EXPLOSIVE_STEPS, mStepSound, true);
}

void EnemyExplosive::Update()
{
    Enemy::Update();
    GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_EXPLOSIVE_STEPS, mStepSound, false);

    GameManager::GetInstance()->GetAudio()->SetPosition(SFX::ENEMY_EXPLOSIVE_STEPS, mStepSound, mGameObject->GetWorldPosition());
}

void EnemyExplosive::Charge()
{
    Enemy::Charge();
    if (!mChargePlaying)
    {
        mExplosionPlaying = false;
        mChargeSound = GameManager::GetInstance()->GetAudio()->Play(SFX::ENEMY_EXPLOSIVE_PREEXPLOSION);
        mChargePlaying = true;
    }

    if (mExplosionWarningGO)
    {
        mExplosionWarningGO->SetEnabled(true);
        ChargeWarningArea();
    }
}

void EnemyExplosive::Attack()
{
    GameManager::GetInstance()->GetPlayerCamera()->ActivateShake(0.5f, 0.2f);
    if (!mExplosionPlaying)
    {
        mChargePlaying = false;
        GameManager::GetInstance()->GetAudio()->Pause(SFX::ENEMY_EXPLOSIVE_PREEXPLOSION, mChargeSound, false);
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::ENEMY_EXPLOSIVE_EXPLOSION, mGameObject->GetWorldPosition());
        mExplosionPlaying = true;
    }

    mExplosionWarningGO->SetWorldScale(float3(0.1f));

    if (mExplosionParticle)
    {
        mExplosionParticle->SetEnabled(true);  
    }

    if (IsPlayerInRange(mExplosionRadius))
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mPlayer->GetComponent(ComponentType::SCRIPT));
        playerScript = static_cast<PlayerController*>(script->GetScriptInstance());

        if (playerScript != nullptr)
        {
            playerScript->TakeDamage(mAttackDamage);
        }
    }

    float explosionRadiusSq = mExplosionRadius * mExplosionRadius;
    const std::vector<GameObject*>& allEnemies = App->GetScene()->FindGameObjectsWithTag("Enemy");
    for (GameObject* enemy : allEnemies)
    {
        if (enemy->IsEnabled())
        {
            if (mGameObject->GetWorldPosition().DistanceSq(enemy->GetWorldPosition()) <= explosionRadiusSq)
            {
                Enemy* enemyScript = static_cast<Enemy*>(static_cast<ScriptComponent*>(enemy->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
                enemyScript->TakeDamage(mAttackDamage);
            }
        }  
    }

    if (mCollider) mCollider->SetEnable(false);
    mCurrentState = EnemyState::DEATH;
}

void EnemyExplosive::Death()
{   
    if (mStepSound != -1) GameManager::GetInstance()->GetAudio()->Release(SFX::ENEMY_EXPLOSIVE_STEPS, mStepSound);
    if (mChargeSound != -1)  GameManager::GetInstance()->GetAudio()->Release(SFX::ENEMY_EXPLOSIVE_PREEXPLOSION, mChargeSound);
    if (mAttackSound != -1)  GameManager::GetInstance()->GetAudio()->Release(SFX::ENEMY_EXPLOSIVE_EXPLOSION, mAttackSound);
    if (mExplosionParticle) mExplosionParticle->SetEnabled(false);

    Enemy::Death();

    mExplosionWarningGO->SetWorldScale(float3::one);
    mExplosionWarningGO->SetEnabled(false);
}

void EnemyExplosive::TakeDamage(float damage)
{
    if (mHealth > 0.0f)
    {
        mHealth = 0.0f;
        mCurrentState = EnemyState::CHARGE;
    }
    
    ActivateHitEffect();
}

void EnemyExplosive::ChargeWarningArea()
{
    if (mExplosionWarningGO)
    {
        float dt = App->GetDt();
        float warningScaleMax = mExplosionRadius * 2.0f; 
        float scaleSpeed = 2.0f; 

        float3 currentScale = mExplosionWarningGO->GetWorldScale();

        float increment = dt * scaleSpeed * warningScaleMax;

        float newScaleX = std::min(currentScale.x + increment, warningScaleMax);
        float newScaleY = std::min(currentScale.y + increment, warningScaleMax);

        float3 newScale = float3(newScaleX, newScaleY, currentScale.z);
        mExplosionWarningGO->SetWorldScale(newScale);
    }
}