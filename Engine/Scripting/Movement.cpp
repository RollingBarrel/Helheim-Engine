#include "pch.h"
#include "Movement.h"
#include <utility>
#include <limits.h>
#include "Globals.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "NavMeshController.h"
#include "Keys.h"

 

Movement::Movement(GameObject* owner) : Script(owner)
{

}

void Movement::Start()
{
    mNavMeshControl = App->GetScene()->GetNavController();
}

void Movement::Update()
{
    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT) {
        float3 newPos = (mGameObject->GetPosition() + float3(0, 0, 1) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT) {
        float3 newPos = (mGameObject->GetPosition() + float3(0, 0, -1) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT) {
        float3 newPos = (mGameObject->GetPosition() + float3(-1, 0, 0) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT) {
        float3 newPos = (mGameObject->GetPosition() + float3(1, 0, 0) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));
    }

}


