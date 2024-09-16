#include "BossLaserEyeBall.h"
#include "Application.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "ScriptComponent.h"
#include "Physics.h"
#include "Geometry/Ray.h"
#include "GameManager.h"
#include "State.h"
#include <MathFunc.h>

CREATE(BossLaserEyeBall)
{
    CLASS(owner);
    SEPARATOR("GAME OBJECTS");
    MEMBER(MemberType::GAMEOBJECT, mLaserOrigin);
    MEMBER(MemberType::GAMEOBJECT, mLaserCharge);
    MEMBER(MemberType::GAMEOBJECT, mLaserEnd);
    END_CREATE;
}

BossLaserEyeBall::BossLaserEyeBall(GameObject* owner) : Script(owner)
{
}

void BossLaserEyeBall::Start()
{
    mElapsedTime = 0.0f;
    mCanDamage = false;

}

void BossLaserEyeBall::Update()
{
    if (GameManager::GetInstance()->IsPaused()) return;

    float deltaTime = App->GetDt();
    mElapsedTime += deltaTime;

    switch (mCurrentState)
    {
    case LaserEyeBallState::IDLE:

        break;

    case LaserEyeBallState::CHARGING:

        Charge();

        if (mElapsedTime >= mAimTime) 
        {
            mCurrentState = LaserEyeBallState::FIRING;
            mElapsedTime = 0.0f; 
        }
        break;

    case LaserEyeBallState::FIRING:
        ActivateLaserVFX();
        RotateLaser();
        UpdateLaser(); 

        if (mElapsedTime >= mDuration) 
        {
            mCurrentState = LaserEyeBallState::COOLDOWN;
            mElapsedTime = 0.0f; 
            DisableLaserVFX();   
        }
        break;

    case LaserEyeBallState::COOLDOWN:
        if (mElapsedTime >= mAttackCoolDown) 
        {
            mCurrentState = LaserEyeBallState::IDLE;
            mGameObject->SetEnabled(false); 
        }
        break;
    }
}


void BossLaserEyeBall::Init(float damage, float duration, float distance, float speed, float rotation)
{
    mDamage = damage;
    mDuration = duration;
    mDistance = distance;
    mRotationSpeed = speed;
    mInitRotation = rotation;
    mCurrentRotation = mInitRotation;

    mAttackCoolDownTimer.Reset(); 

    mCurrentState = LaserEyeBallState::CHARGING;
    mElapsedTime = 0.0f;
    mCanDamage = true;
    DisableLaserVFX();


}

void BossLaserEyeBall::RotateLaser()
{
    float deltaTime = App->GetDt();
    float rotationAmount = mRotationSpeed * deltaTime;

    mCurrentRotation += rotationAmount;

    float maxRotation = mInitRotation + 4.1f;
    float minRotation = mInitRotation - 4.1f;

    if (mCurrentRotation > maxRotation)
    {
        mCurrentRotation = maxRotation;
        mRotationSpeed = -std::abs(mRotationSpeed);
    }
    else if (mCurrentRotation < minRotation)
    {
        mCurrentRotation = minRotation;
        mRotationSpeed = std::abs(mRotationSpeed);
    }

    mGameObject->SetLocalRotation((float3(0, mCurrentRotation, 0)));
}

void BossLaserEyeBall::ActivateLaserVFX()
{
    if (mLaserOrigin) mLaserOrigin->SetEnabled(true);
    if (mLaserEnd) mLaserEnd->SetEnabled(true);
}

void BossLaserEyeBall::DisableLaserVFX()
{
    if (mLaserOrigin) mLaserOrigin->SetEnabled(false);
    if (mLaserCharge) mLaserCharge->SetEnabled(false);
    if (mLaserEnd) mLaserEnd->SetEnabled(false);
}

void BossLaserEyeBall::UpdateLaser()
{
    if (mLaserCharge) mLaserCharge->SetEnabled(false);

    if (mAttackCoolDownTimer.Delay(mAttackCoolDown))
    {
        mCanDamage = true; 
    }

    Hit hit;
    Ray ray;
    ray.dir = mGameObject->GetFront();
    ray.pos = mLaserOrigin->GetWorldPosition();

    Physics::Raycast(hit, ray, mDistance);

    if (mCanDamage && (hit.IsValid()))
    {
        if (hit.mGameObject->GetTag().compare("Player") == 0)
        {
            ScriptComponent* playerScript = static_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
            PlayerController* player = static_cast<PlayerController*>(playerScript->GetScriptInstance());

            if (!(player->GetPlayerLowerState()->GetType() == StateType::DASH))
            {
                player->TakeDamage(mDamage);
                mCanDamage = false;
                mAttackCoolDownTimer.Reset();
            }
        }
        mLaserEnd->SetWorldPosition(hit.mHitPoint);
    }
    else
    {
        float3 originPosition = mLaserOrigin->GetLocalPosition();
        mLaserEnd->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mDistance));
    }
    
}

void BossLaserEyeBall::Charge()
{
  if (mLaserCharge) mLaserCharge->SetEnabled(true); 
}