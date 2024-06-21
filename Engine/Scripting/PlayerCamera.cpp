#include "GameObject.h"
#include "PlayerCamera.h"
#include "CameraComponent.h"
#include "Application.h"
#include <MathFunc.h>


CREATE(PlayerCamera)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mFollowTarget);
    MEMBER(MemberType::FLOAT, mYawAngle);
    MEMBER(MemberType::FLOAT, mPitchAngle);
    MEMBER(MemberType::FLOAT, mDistanceToPlayer);
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
    mGameObject->SetPosition(mFollowTarget->GetPosition());
    mGameObject->SetRotation(float3(DegToRad(mYawAngle), DegToRad(mPitchAngle), 0.0f));
    mGameObject->Translate(-(mGameObject->GetFront()) * mDistanceToPlayer);
}

void PlayerCamera::Update()
{
    // TODO: Change to offset when camera values are defined.

    mGameObject->SetPosition(mFollowTarget->GetPosition());
    mGameObject->SetRotation(float3(DegToRad(mYawAngle), DegToRad(mPitchAngle), 0.0f));
    mGameObject->Translate(-(mGameObject->GetFront())*mDistanceToPlayer);
}

