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
    mCurrentRotation = 89.5;

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

void BossLaserEyeBall::Init(float distance, float duration)
{
    mDistance = distance;
    mDuration = duration;
    mRotationSpeed = 1.0f;
}



void BossLaserEyeBall::RotateLaser()
{
    float deltaTime = App->GetDt();
    float rotationAmount = mRotationSpeed * deltaTime;

    mCurrentRotation += rotationAmount;
    LOG("Current Rotation: %f", mCurrentRotation);

    float maxRotation = 90.f;
    float minRotation = 89.f;

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
        if (mLaserOrigin) mLaserOrigin->SetEnabled(true);
        if (mLaserTrail) mLaserTrail->SetEnabled(true);
        if (mLaserEnd) mLaserEnd->SetEnabled(true);

        Hit hit;
        Ray ray;
        ray.dir = float3(std::sin(DegToRad(mCurrentRotation)), 0.0f, std::cos(DegToRad(mCurrentRotation)));
        ray.pos = mLaserOrigin->GetWorldPosition();

        Physics::Raycast(hit, ray, mDistance);

        if (hit.IsValid())
        {
            if (hit.mGameObject->GetTag().compare("Player") == 0)
            {
                ScriptComponent* playerScript = static_cast<ScriptComponent*>(GameManager::GetInstance()->GetPlayer()->GetComponent(ComponentType::SCRIPT));
                PlayerController* player = static_cast<PlayerController*>(playerScript->GetScriptInstance());

                if (!player->GetPlayerLowerState()->GetType() == StateType::DASH) 
                {
                    player->TakeDamage(mDamage);
                }
            }
        }
        else
        {
            float3 originPosition = mLaserOrigin->GetLocalPosition();
            mLaserEnd->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mDistance));

            if (mMoveTrail)
            {
                mMoveTrail = false;
                mLaserTrail->SetLocalPosition(float3(originPosition.x, originPosition.y, originPosition.z + mDistance));

            }
            else
            {
                    
                mMoveTrail = true;
                mLaserTrail->SetLocalPosition(originPosition);

            }
        }
        
    }
    else
    {
        if (mLaserOrigin) mLaserOrigin->SetEnabled(false);
        if (mLaserTrail) mLaserTrail->SetEnabled(false);
        if (mLaserEnd) mLaserEnd->SetEnabled(false);
    }
}