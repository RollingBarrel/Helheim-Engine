#include "pch.h"
#include "PlayerController.h"
#include <utility>
#include <limits.h>
#include "Globals.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "NavMeshController.h"
#include "Keys.h"
#include "Math/MathFunc.h"

PlayerController::PlayerController(GameObject* owner) : Script(owner)
{

}

void PlayerController::Start()
{
    mNavMeshControl = App->GetScene()->GetNavController();
}

void PlayerController::Update()
{
    Move();
    Rotate();
    Win();


}

void PlayerController::Move() {
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

void PlayerController::Win() {
    if (mWinArea) {
     float3 winPosition= mWinArea->GetPosition();
     float3 playerPosition = mGameObject->GetPosition();
     if (((winPosition.x - playerPosition.x)<2 && (winPosition.x - playerPosition.x)> -2) && ((winPosition.z - playerPosition.z) < 2 && (winPosition.z - playerPosition.z) > -2)) {
         mGameObject->SetPosition(float3::zero);
     }

    }

}

void PlayerController::Rotate() {

        int mX, mY;
        App->GetInput()->GetMouseMotion(mX, mY);
        float3 rotation = { 0.0f, DegToRad(mX * mPlayerRotationSpeed), 0.0f };
        mGameObject->SetRotation(rotation+ mGameObject->GetRotation());


}


