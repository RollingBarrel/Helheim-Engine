#include "PlayerCamera.h"
#include "GameObject.h"

#include <MathFunc.h>

#include "Application.h"

#include "ModuleInput.h"


CREATE(PlayerCamera)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mFollowTarget);
    MEMBER(MemberType::FLOAT, mYawAngle);
    MEMBER(MemberType::FLOAT, mPitchAngle);
    MEMBER(MemberType::FLOAT, mDistanceToPlayer);
    END_CREATE;
}

void PlayerCamera::Start()
{
    const std::vector<GameObject*> children = mGameObject->GetChildren();
    if (!children.empty()) mCameraObject = mGameObject->GetChildren()[0];

    mGameObject->SetWorldPosition(mFollowTarget->GetWorldPosition());
    mGameObject->SetWorldRotation(float3(DegToRad(mYawAngle), DegToRad(mPitchAngle), 0.0f));
    mGameObject->Translate(-(mGameObject->GetFront()) * mDistanceToPlayer);
}

void PlayerCamera::Update()
{
    mGameObject->SetWorldPosition(mFollowTarget->GetWorldPosition());
    mGameObject->SetWorldRotation(float3(DegToRad(mYawAngle), DegToRad(mPitchAngle), 0.0f));
    mGameObject->Translate(-(mGameObject->GetFront())*mDistanceToPlayer);

    if (!mShakeTimer.Delay(mShakeDuration))
    {
        Shake();
    }
    else
    {
        if (mCameraObject) mCameraObject->SetLocalPosition(float3::zero);
        mShakeDuration = 0.0f;
    }
}

void PlayerCamera::ActivateShake(float duration, float positionOffsetStrength)
{
    mShakeDuration = duration;
    mShakePositionOffsetStrength = positionOffsetStrength;
    mShakeTimer.Reset();
}

void PlayerCamera::Shake()
{
    if (mCameraObject)
    {
        float randomX = ((float)rand()) / (float)RAND_MAX * mShakePositionOffsetStrength;
        float randomY = ((float)rand()) / (float)RAND_MAX * mShakePositionOffsetStrength;

        mCameraObject->SetLocalPosition(float3(randomX,randomY,0.0f));
    }
}

