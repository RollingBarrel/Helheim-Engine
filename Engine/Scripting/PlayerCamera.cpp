#include "GameObject.h"
#include "PlayerCamera.h"
#include "CameraComponent.h"
#include "Application.h"


CREATE(PlayerCamera)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mLookTarget);
    MEMBER(MemberType::GAMEOBJECT, mFollowTarget);
    MEMBER(MemberType::FLOAT3, mCameraPosition);
    MEMBER(MemberType::FLOAT3, mCameraRotation);
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
        float3 actualPosition = mGameObject->GetPosition();
        mGameObject->SetPosition(float3(mFollowTarget->GetPosition().x, actualPosition.y, actualPosition.z));
    }
    //look at the target
    if (mLookTarget && mCameraComponent)
    {
        mGameObject->LookAt(mLookTarget->GetPosition());
    }


    SetView(mCameraPosition, mCameraRotation);


}

void PlayerCamera::Update()
{
    //CameraManager();


    float transitionTime = 1.0f;
    static float deltaTime = transitionTime;

    float3 position = mFollowTarget->GetPosition() - mGameObject->GetPosition();

    if ((position - mCameraPosition).Abs().Length() > 0.9)
    {
        float3 direction = mCameraPosition - position;
        deltaTime = deltaTime - App->GetDt(); // Quizas seria mejor usar el GameManager en vez de App
        float3 rotation = mCameraRotation - mGameObject->GetRotation();
        if (deltaTime > 0) 
        {
            float factor = (transitionTime - deltaTime) / transitionTime;
            SetView(position + direction * factor, mGameObject->GetRotation()+ rotation * factor);
        }
    }
    else
    {
        deltaTime = transitionTime;
        SetView(mCameraPosition, mCameraRotation);
    }


}

void PlayerCamera::SetView(float3 position, float3 rotation)
{
    float3 newPosition = mFollowTarget->GetPosition() - position;

    mGameObject->SetPosition(newPosition);
    mGameObject->SetRotation(rotation);
}
