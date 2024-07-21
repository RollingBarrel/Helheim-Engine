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
    mExplosionWarningGO->SetEnabled(true);
    ChargeWarningArea();
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
    float increment = (mExplosionRadius - mExplosionWarningGO->GetWorldScale().x) * App->GetDt();
    float3 newWarningSize = float3(mWarningSize.x += increment, mWarningSize.y, mWarningSize.z += increment);
    mExplosionWarningGO->SetWorldScale(newWarningSize);
}
