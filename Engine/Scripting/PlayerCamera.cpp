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

    SetView(float3(0, -10.0f, 7.0f), 0.5f);


}

void PlayerCamera::Update()
{
    CameraManager();

    float3 closePosition = float3(0, -5.0f, 7.0f);
    float closeRotation = 0.5f;
    float3 topPosition = float3(0, -8.0f, 16.0f);
    float topRotation = 0.3f;

    float transitionTime = 1.0f;
    static float deltaTime = transitionTime;
        
    float3 position = mFollowTarget->GetWorldPosition() - mGameObject->GetPosition();

    switch (mCameraMode) 
    {
    case CameraMode::CLOSE:
        if ((position - closePosition).Abs().Length() > 0.7)
        {
            LOG("distance = %f", (position - closePosition).Abs().Length());
            float3 direction = closePosition - position;
            deltaTime = deltaTime - App->GetGameDt(); // Quizas seria mejor usar el GameManager en vez de App
            float rotation = closeRotation - mGameObject->GetRotation().x;
            if (deltaTime > 0) {
                float factor = (transitionTime-deltaTime) / transitionTime;
                SetView(position + direction * factor, mGameObject->GetRotation().x + rotation * factor);
            }
        }
        else 
        {
            deltaTime = transitionTime;
            SetView(closePosition, closeRotation);
        }
        break;

    case CameraMode::TOP:
        if ((position - topPosition).Abs().Length() > 0.9)
        {
            LOG("distance = %f", (position - topPosition).Abs().Length());
            float3 direction = topPosition - position;
            deltaTime = deltaTime - App->GetGameDt(); // Quizas seria mejor usar el GameManager en vez de App
            float rotation = topRotation - mGameObject->GetRotation().x;
            if (deltaTime > 0) {
                float factor = (transitionTime - deltaTime) / transitionTime;
                SetView(position + direction * factor, mGameObject->GetRotation().x + rotation * factor);
            }
        }
        else
        {
            deltaTime = transitionTime;
            SetView(topPosition, topRotation);
        }
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

void PlayerCamera::SetView(float3 position, float rotation)
{
    float3 newPosition = mFollowTarget->GetWorldPosition() - position;

    mGameObject->SetPosition(newPosition);
    mGameObject->SetRotation(float3(rotation, 0.0f, 0.0f));
}
