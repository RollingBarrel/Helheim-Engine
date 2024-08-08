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
    mCurrentRotation = 0.0f;
    mRotatingRight = true;

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
        mGameObject->SetEnabled(false);
        return;
    }
    else
    {
        RotateLaser();
    }
}

void BossLaserEyeBall::Init(float damage, float distance, float duration, float rotationSpeed)
{
    mDamage = damage;
    mDistance = distance;
    mDuration = duration;
    mRotationSpeed = rotationSpeed;
    mCurrentRotation = 0.0f;
    mRotatingRight = true;
    mElapsedTime = 0.0f;

    if (mLaserOrigin) mLaserOrigin->SetEnabled(true);
    if (mLaserTrail) mLaserTrail->SetEnabled(true);
    if (mLaserEnd) mLaserEnd->SetEnabled(true);

}
void BossLaserEyeBall::RotateLaser()
{
    float deltaTime = App->GetDt();
    float rotationAmount = mRotationSpeed * deltaTime;

    // Update current rotation with respect to rotation direction
    if (mRotatingRight)
    {
        mCurrentRotation += rotationAmount;
        if (mCurrentRotation >= MAX_ROTATION)
        {
            mCurrentRotation = MAX_ROTATION;
            mRotatingRight = false; // Change direction
        }
    }
    else
    {
        mCurrentRotation -= rotationAmount;
        if (mCurrentRotation <= MIN_ROTATION)
        {
            mCurrentRotation = MIN_ROTATION;
            mRotatingRight = true; // Change direction
        }
    }

    // Apply rotation to the laser
    float3 currentEulerAngles = mGameObject->GetLocalEulerAngles();
    currentEulerAngles.y = mCurrentRotation;
    mGameObject->SetLocalRotation(currentEulerAngles);

    if (mLaserOrigin && mLaserEnd)
    {
        Hit hit;
        Ray ray;
        ray.dir = float3(std::sin(DegToRad(mCurrentRotation)), 0.0f, std::cos(DegToRad(mCurrentRotation)));
        ray.pos = mLaserOrigin->GetWorldPosition();

        Physics::Raycast(hit, ray, mDistance);

        if (hit.IsValid())
        {
            if (hit.mGameObject->GetTag().compare("Player") == 0)
            {
                ScriptComponent* playerScript = static_cast<ScriptComponent*>(hit.mGameObject->GetComponent(ComponentType::SCRIPT));
                PlayerController* player = static_cast<PlayerController*>(playerScript->GetScriptInstance());
                player->TakeDamage(mDamage);
            }

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
