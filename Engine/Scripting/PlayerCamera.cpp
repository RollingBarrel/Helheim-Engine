#include "pch.h"
#include "GameObject.h"
#include "PlayerCamera.h"
#include "CameraComponent.h"

CREATE(PlayerCamera)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mLookTarget);
    MEMBER(MemberType::GAMEOBJECT, mFollowTarget);
    END_CREATE;
}

PlayerCamera::PlayerCamera(GameObject* owner) : Script(owner)
{
}

void PlayerCamera::Start()
{
    mCameraComponent = reinterpret_cast<CameraComponent*>(mGameObject->GetComponent(ComponentType::CAMERA));
}

void PlayerCamera::Update()
{
    if (mFollowTarget)
    {
        float3 actualPosition = mGameObject->GetPosition();
        mGameObject->SetPosition(float3(mFollowTarget->GetPosition().x, actualPosition.y, actualPosition.z));
    }

    if (mLookTarget && mCameraComponent)
    {
        mCameraComponent->LookAt(mGameObject->GetPosition(), mLookTarget->GetPosition(), mGameObject->GetUp());
    }
}
