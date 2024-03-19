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
#include "AnimationComponent.h"

PlayerController::PlayerController(GameObject* owner) : Script(owner)
{

}

void PlayerController::Start()
{
    mNavMeshControl = App->GetScene()->GetNavController();

    if (mAnimationComponentHolder) {
        mAnimationComponent = (AnimationComponent*)mAnimationComponentHolder->GetComponent(ComponentType::ANIMATION);
        mAnimationComponent->OnStart();
    }
}

void PlayerController::Update()
{
    Move();
    Rotate();
    Win();
    Dash();

    if (mAnimationComponent) {
        mAnimationComponent->OnUpdate();
    }

}

void PlayerController::Move() {
    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT) {
        float3 newPos = (mGameObject->GetPosition() + float3(0, 0, 1) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT) {
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
     if (((winPosition.x - playerPosition.x)<5 && (winPosition.x - playerPosition.x)> -5) && ((winPosition.z - playerPosition.z) < 5 && (winPosition.z - playerPosition.z) > -5)) {
         //mGameObject->SetPosition(float3::zero);
         LOG("WIN");
     }

    }

}

void PlayerController::Rotate() {

        int mX, mY;
        App->GetInput()->GetMouseMotion(mX, mY);
        float3 rotation = { 0.0f, DegToRad(mX * mPlayerRotationSpeed), 0.0f };
        mGameObject->SetRotation(rotation+ mGameObject->GetRotation());


}

void PlayerController::Dash()
{
    if (App->GetInput()->GetKey(Keys::Keys_T) == KeyState::KEY_DOWN && !mStartCounter)
    {
        mIsDashActive = true;
    }
    if (mIsDashActive)
    {
        if (mDashMovement >= mDashLenght)
        {
            mStartCounter = false;
            mDashMovement = 0;
            mIsDashActive = false;
        }
        if (mStartCounter)
        {
            mDashTimePassed += App->GetGameDt();
            if (mDashTimePassed >= mDashCoolDown)
            {
                mDashTimePassed = 0;
                mStartCounter = true;
             
            }
        }
        else
        {
            mDashMovement += mDashSpeed * App->GetGameDt();
            //mGameObject->SetPosition(mGameObject->GetPosition() + mGameObject->GetFront() * mDashSpeed * App->GetGameDt());
            float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * App->GetGameDt() * mDashSpeed);
            mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));

        }
    }


}


