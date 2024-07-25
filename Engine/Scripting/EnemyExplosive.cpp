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
    END_CREATE;
}

void EnemyExplosive::Start()
{
    Enemy::Start();

    if (mExplosionWarningGO)
    {
        mWarningSize = mExplosionWarningGO->GetWorldScale();
        mExplosionWarningGO->SetLocalPosition(float3(0.0f, 0.1f, 0.0f));
        mExplosionWarningGO->SetEnabled(false);
    }

    GameObject* firstChild = *(mGameObject->GetChildren().begin());
    if (firstChild)
    {
      mExplosionParticles = reinterpret_cast<ParticleSystemComponent*>(firstChild->GetComponent(ComponentType::PARTICLESYSTEM));
      if (mExplosionParticles)
      {
          mExplosionParticles->SetEnable(false);
      }
     }
}


void EnemyExplosive::Charge()
{
    Enemy::Charge();
    if (mExplosionWarningGO)
    {
        mExplosionWarningGO->SetEnabled(true);
        ChargeWarningArea();
    }
}

void EnemyExplosive::Attack()
{
    mExplosionWarningGO->SetWorldScale(float3(0.1f));

    if (IsPlayerInRange(mExplosionRadius))
    {
        if (mExplosionParticles)
        {
            mExplosionParticles->SetEnable(true);
        }

        PlayerController* playerScript = (PlayerController*)((ScriptComponent*)mPlayer->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
        if (playerScript != nullptr)
        {
            playerScript->TakeDamage(mAttackDamage);
        }
    }
    else
    {
        mExplosionWarningGO->SetEnabled(false);
    }
    TakeDamage(mMaxHealth);
}

void EnemyExplosive::ChargeWarningArea()
{
    if (mExplosionWarningGO)
    {
        float dt = App->GetDt();
        float warningScaleMax = mExplosionRadius * 2.0f; // Final scale (diameter)
        float scaleSpeed = 0.5f; // Speed factor for how fast the warning grows

        // Current scale
        float3 currentScale = mExplosionWarningGO->GetWorldScale();

        // Calculate the increment based on dt and scaleSpeed
        float increment = dt * scaleSpeed * warningScaleMax;

        // Calculate new scale
        float newScaleX = std::min(currentScale.x + increment, warningScaleMax);
        float newScaleZ = std::min(currentScale.z + increment, warningScaleMax);

        // Set the new scale, keeping Y scale constant
        float3 newScale = float3(newScaleX, currentScale.y, newScaleZ);
        mExplosionWarningGO->SetWorldScale(newScale);
    }
}