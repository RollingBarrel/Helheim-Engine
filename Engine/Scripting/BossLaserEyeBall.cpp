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
    SEPARATOR("STATS");
    MEMBER(MemberType::FLOAT, mDamage);
    END_CREATE;
}

BossLaserEyeBall::BossLaserEyeBall(GameObject* owner) : Script(owner), mMoveTrail(false)
{
}

void BossLaserEyeBall::Start()
{
    mElapsedTime = 0.0f;
    mCurrentRotation = 90.0f;

    if (mLaserOrigin) mLaserOrigin->SetEnabled(false);
    if (mLaserCharge)
    {
        mLaserCharge->SetEnabled(false);
        if (mLaserOrigin) mLaserCharge->SetLocalPosition(mLaserOrigin->GetLocalPosition());
    }
    if (mLaserEnd) mLaserEnd->SetEnabled(false);
}

void BossLaserEyeBall::Update()
{
    float deltaTime = App->GetDt();
    mElapsedTime += deltaTime;

    if (mElapsedTime >= mDuration)
    {
        DisableLaserVFX();
        mGameObject->SetEnabled(false);
        return;
    }

    RotateLaser();  
}

void BossLaserEyeBall::Init(float distance, float duration, float rotation, float rotationDirection)
{
    mDistance = distance;
    mDuration = duration;
    mRotationSpeed = 1.0f * rotationDirection;
    mInitRotation = rotation;
    mCurrentRotation = mInitRotation;
    mRotationDirection = rotationDirection;
}

void BossLaserEyeBall::RotateLaser()
{
    float deltaTime = App->GetDt();
    float rotationAmount = mRotationSpeed * deltaTime;

    mCurrentRotation += rotationAmount;

    float maxRotation = mInitRotation + 1.f;
    float minRotation = mInitRotation - 1.f;

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

    if (mCurrentRotation >= minRotation && mCurrentRotation <= maxRotation)
    {
        ActivateLaserVFX();
        UpdateLaser();
    }
    else
    {
        DisableLaserVFX();
    }
}

void BossLaserEyeBall::ActivateLaserVFX()
{
    if (mLaserOrigin) mLaserOrigin->SetEnabled(true);
    if (mLaserCharge) mLaserCharge->SetEnabled(true);
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
    Hit hit;
    Ray ray;
    ray.dir = mGameObject->GetFront(); 
    ray.pos = mLaserOrigin->GetWorldPosition();

    Physics::Raycast(hit, ray, mDistance);

    if (hit.IsValid())
    {
        if (hit.mGameObject->GetTag().compare("Player") == 0)
        {
            ScriptComponent* playerScript = static_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
            PlayerController* player = static_cast<PlayerController*>(playerScript->GetScriptInstance());

            if (!(player->GetPlayerLowerState()->GetType() == StateType::DASH))
            {
                player->TakeDamage(mDamage);
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
