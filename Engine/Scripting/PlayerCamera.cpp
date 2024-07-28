#include "GameObject.h"
#include "PlayerCamera.h"
#include "Application.h"
#include <MathFunc.h>
#include "Gamemanager.h"
#include "CinematicCamera.h"

CREATE(PlayerCamera)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mFollowTarget);
    MEMBER(MemberType::FLOAT, mYawAngle);
    MEMBER(MemberType::FLOAT, mPitchAngle);
    MEMBER(MemberType::FLOAT, mDistanceToPlayer);
    MEMBER(MemberType::FLOAT, mSpeedFactor);
    END_CREATE;
}

PlayerCamera::PlayerCamera(GameObject* owner) : Script(owner)
{
}

void PlayerCamera::Awake()
{
}

void PlayerCamera::Start()
{
    mTargetPosition = ((mFollowTarget->GetWorldPosition()) - ((mGameObject->GetFront()) * mDistanceToPlayer));
    mGameObject->SetWorldPosition(float3(92.98f, 0.0f, 0.0f));
    mGameObject->SetWorldRotation(float3(DegToRad(mYawAngle), DegToRad(mPitchAngle), 0.0f));
    mGameObject->Translate(-(mGameObject->GetFront()) * mDistanceToPlayer);
}

void PlayerCamera::Update()
{
    float deltaTime = App->GetDt();

    if (!mMoveCompleted)
    {
        float3 currentPosition = mGameObject->GetWorldPosition();
        float3 direction = mTargetPosition - currentPosition;
        float distance = direction.Length();
        direction.Normalize();

        float step = mSpeedFactor * deltaTime;

        GameManager::GetInstance()->SetPaused(true, false);

        if (step >= distance)
        {
            mGameObject->SetWorldPosition(mTargetPosition);
            mMoveCompleted = true;

            GameManager::GetInstance()->SetPaused(false, false);
        }
        else
        {
            mGameObject->SetWorldPosition(lerp(currentPosition, mTargetPosition, mSpeedFactor * deltaTime));
        }
    }
    else
    {
        mGameObject->SetWorldPosition(mFollowTarget->GetWorldPosition());
        mGameObject->SetWorldRotation(float3(DegToRad(mYawAngle), DegToRad(mPitchAngle), 0.0f));
        mGameObject->Translate(-(mGameObject->GetFront()) * mDistanceToPlayer);
    }
}

float3 PlayerCamera::lerp(const float3& start, const float3& end, float t)
{
    return float3{
        start.x + t * (end.x - start.x),
        start.y + t * (end.y - start.y),
        start.z + t * (end.z - start.z)
    };
}