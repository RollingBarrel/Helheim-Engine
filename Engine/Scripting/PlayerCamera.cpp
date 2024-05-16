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
    //scene->FindGameObjectsWithTag(scene->GetRoot(), scene->GetTagByName("CombatArea")->GetID(), mCombatAreas);


    //follow the target
    if (mFollowTarget)
    {
        float3 actualPosition = mGameObject->GetWorldPosition();
        mGameObject->SetPosition(float3(mFollowTarget->GetWorldPosition().x, actualPosition.y, actualPosition.z));
    }
    //look at the target
    if (mLookTarget && mCameraComponent)
    {
        mGameObject->LookAt(mLookTarget->GetWorldPosition());
    }

    mAuxLookTarget = mLookTarget;

    SetView(float3(-5.0f, -10.0f, 16.0f), 0.4f);


}

void PlayerCamera::Update()
{
    //CameraManager();

    float3 topPosition = float3(-5.0f, -10.0f, 16.0f);
    float topRotation = 0.4f;

    float transitionTime = 1.0f;
    static float deltaTime = transitionTime;

    float3 position = mFollowTarget->GetWorldPosition() - mGameObject->GetPosition();

    if ((position - topPosition).Abs().Length() > 0.9)
    {
        float3 direction = topPosition - position;
        deltaTime = deltaTime - App->GetDt(); // Quizas seria mejor usar el GameManager en vez de App
        float rotation = topRotation - mGameObject->GetRotation().x;
        if (deltaTime > 0) 
        {
            float factor = (transitionTime - deltaTime) / transitionTime;
            SetView(position + direction * factor, mGameObject->GetRotation().x + rotation * factor);
        }
    }
    else
    {
        deltaTime = transitionTime;
        SetView(topPosition, topRotation);
    }


}

//float3 PlayerCamera::GetCameraDirection() const
//{
//    // Ritorna la direzione della telecamera rispetto al player
//    if (mCameraComponent && mFollowTarget)
//    {
//        return (mLookTarget->GetWorldPosition() - mFollowTarget->GetWorldPosition()).Normalized();
//    }
//    return float3::zero;
//}

void PlayerCamera::SetView(float3 position, float rotation)
{
    float3 newPosition = mFollowTarget->GetWorldPosition() - position;

    mGameObject->SetPosition(newPosition);
    mGameObject->SetRotation(float3(0.3f, -0.39f, 0.19f));
}
