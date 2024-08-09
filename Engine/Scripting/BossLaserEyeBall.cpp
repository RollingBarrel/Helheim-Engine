#include "BossLaserEyeBall.h"
#include "Application.h"
#include "GameObject.h"
#include "PlayerController.h"
#include "ScriptComponent.h"
#include "Physics.h"
#include "Geometry/Ray.h"
#include <MathFunc.h>

CREATE(BossLaserEyeBall)
{
    CLASS(owner);
    SEPARATOR("GAME OBJECTS");
    MEMBER(MemberType::GAMEOBJECT, mLaserOrigin);
    MEMBER(MemberType::GAMEOBJECT, mLaserTrail);
    MEMBER(MemberType::GAMEOBJECT, mLaserEnd);
    END_CREATE;
}

BossLaserEyeBall::BossLaserEyeBall(GameObject* owner) : Script(owner)
{
}

void BossLaserEyeBall::Start()
{
    mElapsedTime = 0.0f;
    mCurrentRotation = mInitialRotation;

    if (mLaserOrigin) mLaserOrigin->SetEnabled(false);
    if (mLaserTrail) mLaserTrail->SetEnabled(false);
    if (mLaserEnd) mLaserEnd->SetEnabled(false);
}

void BossLaserEyeBall::Update()
{
    float deltaTime = App->GetDt();
    mElapsedTime += deltaTime;

    if (mElapsedTime >= mDuration)
    {
        if (mLaserOrigin) mLaserOrigin->SetEnabled(false);
        if (mLaserTrail) mLaserTrail->SetEnabled(false);
        if (mLaserEnd) mLaserEnd->SetEnabled(false);
        mGameObject->SetEnabled(false);
        return;
    }
        RotateLaser();
}

void BossLaserEyeBall::Init(float distance, float duration, float initialRotation)
{
    mDistance = distance;
    mDuration = duration;
    mInitialRotation = initialRotation;
    mElapsedTime = 0.0f;

    if (mLaserOrigin) mLaserOrigin->SetEnabled(true);
    if (mLaserTrail) mLaserTrail->SetEnabled(true);
    if (mLaserEnd) mLaserEnd->SetEnabled(true);
}

void BossLaserEyeBall::RotateLaser()
{
    float deltaTime = App->GetDt();
    float rotationAmount = mRotationSpeed * deltaTime;

    mCurrentRotation += rotationAmount;

    const float rotationRange = 45.0f; 
    float maxRotation = mInitialRotation + rotationRange;
    float minRotation = mInitialRotation - rotationRange;

    if (mRotationSpeed > 0) 
    {
        if (mCurrentRotation >= maxRotation)
        {
            mCurrentRotation = maxRotation;
            mRotationSpeed = -std::abs(mRotationSpeed);
        }
    }
    else 
    {
        if (mCurrentRotation <= minRotation)
        {
            mCurrentRotation = minRotation;
            mRotationSpeed = std::abs(mRotationSpeed);
        }
    }

    mGameObject->SetLocalRotation(float3(0, mCurrentRotation, 0));

    if (mLaserOrigin && mLaserEnd)
    {
        Hit hit;
        Ray ray;
        ray.dir = float3(std::sin(DegToRad(mCurrentRotation)), 0.0f, std::cos(DegToRad(mCurrentRotation)));
        ray.pos = mLaserOrigin->GetWorldPosition();

        Physics::Raycast(hit, ray, mDistance);

        if (hit.IsValid())
        {
            mLaserEnd->SetWorldPosition(hit.mHitPoint);

            if (mMoveTrail)
            {
                mLaserTrail->SetWorldPosition(hit.mHitPoint);
                mMoveTrail = false;
            }
            else
            {
                mMoveTrail = true;
                mLaserTrail->SetWorldPosition(mLaserOrigin->GetWorldPosition());
            }
        }
        else
        {
            float3 originPosition = mLaserOrigin->GetLocalPosition();
            mLaserEnd->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mDistance));

            if (mMoveTrail)
            {
                mLaserTrail->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mDistance));
                mMoveTrail = false;
            }
            else
            {
                mMoveTrail = true;
                mLaserTrail->SetLocalPosition(originPosition);
            }
        }
    }
}
