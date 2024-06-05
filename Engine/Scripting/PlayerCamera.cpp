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

void PlayerCamera::Start()
{
}

void PlayerCamera::Update()
{


    
    mGameObject->SetPosition(mFollowTarget->GetWorldPosition());
    mGameObject->SetRotation(float3(DegToRad(mYawAngle), DegToRad(mPitchAngle), 0.0f));
    mGameObject->Translate(-(mGameObject->GetFront())*mDistanceToPlayer);


}

