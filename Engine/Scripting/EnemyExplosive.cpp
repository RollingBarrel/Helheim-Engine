#include "EnemyExplosive.h"
#include "Application.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "PlayerController.h"
#include "ParticleSystemComponent.h"


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
    mHealth = 100;

    if (mExplosionWarningGO)
    {
        mWarningSize = mExplosionWarningGO->GetWorldScale();
        mExplosionWarningGO->SetLocalPosition(float3(0.0f, 0.1f, 0.0f));
        mExplosionWarningGO->SetEnabled(false);
        mExplosionParticle->SetEnabled(false);
    }
}


void EnemyExplosive::Charge()
{
    Enemy::Charge();
    if (mExplosionWarningGO)
    {
        mExplosionWarningGO->SetEnabled(true);
        ChargeWarningArea();
        mExplosionParticle->SetEnabled(true);
    }
}

void EnemyExplosive::Attack()
{
    mExplosionWarningGO->SetWorldScale(float3(0.1f));

    if (IsPlayerInRange(mExplosionRadius))
    {
        PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
        if (playerScript != nullptr)
        {
            playerScript->TakeDamage(mAttackDamage);
        }
    }

    TakeDamage(mMaxHealth);
}

void EnemyExplosive::ChargeWarningArea()
{
    if (mExplosionWarningGO)
    {
        float dt = App->GetDt();
        float warningScaleMax = mExplosionRadius * 2.0f; 
        float scaleSpeed = 0.50f; 

        float3 currentScale = mExplosionWarningGO->GetWorldScale();

        float increment = dt * scaleSpeed * warningScaleMax;

        float newScaleX = std::min(currentScale.x + increment, warningScaleMax);
        float newScaleY = std::min(currentScale.y + increment, warningScaleMax);

        float3 newScale = float3(newScaleX, newScaleY, currentScale.z);
        mExplosionWarningGO->SetWorldScale(newScale);
    }
}