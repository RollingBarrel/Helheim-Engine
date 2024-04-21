#include "pch.h"
#include "GameObject.h"
#include "PlayerCamera.h"
#include "CameraComponent.h"
#include "ModuleScene.h"
#include "Application.h"


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

   ModuleScene* scene = App->GetScene();
   scene->FindGameObjectsWithTag(scene->GetRoot(), scene->GetTagByName("CombatArea")->GetID(), mCombatAreas);


    //follow the target
    if (mFollowTarget)
    {
        float3 actualPosition = mGameObject->GetWorldPosition();
        mGameObject->SetPosition(float3(mFollowTarget->GetWorldPosition().x, actualPosition.y, actualPosition.z));
    }
    //look at the target
    if (mLookTarget && mCameraComponent)
    {
        mCameraComponent->LookAt(mGameObject->GetWorldPosition(), mLookTarget->GetWorldPosition(), mGameObject->GetUp());
    }

    mAuxLookTarget = mLookTarget;

    SetView(12.0f, 16.0f, 45.0f);


}

void PlayerCamera::Update()
{
    CameraManager();

    switch (mCameraMode) 
    {
    case CameraMode::CLOSE:
        SetView(10.0f, 7.0f, 0.5f);
        break;

    case CameraMode::TOP:
        SetView(12.0f, 16.0f, 45.0f);
        break;
    }

	
}

//if the target is inside a cube with tag "CombatArea" the camera will move to the top of the cube
void PlayerCamera::CameraManager()
{
    float3 playerPosition = mFollowTarget->GetWorldPosition();

    for (auto area : mCombatAreas)
    {
        float3 areaPosition = area->GetPosition();
        float3 areaSize = area->GetScale();
		if (playerPosition.x > areaPosition.x - areaSize.x / 2 && playerPosition.x < areaPosition.x + areaSize.x / 2 &&
			playerPosition.z > areaPosition.z - areaSize.z / 2 && playerPosition.z < areaPosition.z + areaSize.z / 2)
		{
			mCameraMode = CameraMode::TOP;
            mLookTarget = area; 

			return;
		} 
        else 
        {
            mLookTarget = mAuxLookTarget;
            mCameraMode = CameraMode::CLOSE;
        }

    }
}

void PlayerCamera::SetView(float distance, float heightOffset, float rotation) 
{
    float3 newPosition = mFollowTarget->GetWorldPosition() - float3(0.0f, -heightOffset, distance);

    mGameObject->SetPosition(newPosition);
    mGameObject->SetRotation(float3(rotation, 0.0f, 0.0f));
}
