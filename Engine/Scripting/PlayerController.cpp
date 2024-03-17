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

const float pi = (float)3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;


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
float DegToRad(float degrees) { return degrees * (pi / 180.f); }
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
    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
    {
        int mX, mY;
        App->GetInput()->GetMouseMotion(mX, mY);
        float rotation = DegToRad(mX * mPlayerRotationSpeed);
        float3 rotMatrix = { 0.0f,0.0f, rotation };


        mGameObject->SetRotation(rotMatrix);
    }
}


