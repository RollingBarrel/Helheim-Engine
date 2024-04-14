#include "pch.h"
#include "PlayerController.h"
#include <utility>
#include <limits.h>
#include "Globals.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "AudioSourceComponent.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "Math/MathFunc.h"
#include "AnimationComponent.h"
#include "Geometry/Ray.h"

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

    if (mAudioSourceComponentHolder) {
        mAudioSourceComponent = (AudioSourceComponent*)mAudioSourceComponentHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }
}

void PlayerController::Update()
{
    Move();
    Rotate();
    Win();
    Lose();
    Dash();
    CheckRoute();

    if (mAnimationComponent) {
        mAnimationComponent->OnUpdate();
    }

    if (mAudioSourceComponent) {
        if (!mStepPlayed) {
            mAudioSourceComponent->OnUpdate(mStepAudio);
            mStepPlayed = true;
            mStepStartCounter = true;
        }
        else {
            if (mStepStartCounter) {
                mStepTimePassed += App->GetGameDt();

                if (mStepTimePassed >= mStepCoolDown)
                {
                    mDashTimePassed = 0;
                    mStepStartCounter = false;
                    mStepPlayed = false;
                    mStepTimePassed = 0;

                }
            }
        }
        
    }

    std::map<float, GameObject*> hits;

    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN) {
        
        Ray ray;
        ray.pos = mGameObject->GetPosition();
        ray.dir = mGameObject->GetFront();
        float distance = 100;
        hits = Physics::Raycast(&ray);
        Debug::DrawLine(ray.pos, ray.dir*distance, float3(1.0f, 0.0f, 0.0f));
        for (const auto& hit : hits)
        {
            LOG("Object %s has been hit at distance: %f", hit.second->GetName().c_str(), hit.first);
        }
    }

}

void PlayerController::CheckRoute() 
{
    if (App->GetInput()->GetKey(Keys::Keys_P) == KeyState::KEY_REPEAT) 
    {
        float3 winPosition = mWinArea->GetPosition();
        float3 playerPosition = mGameObject->GetPosition();
        std::vector<float3> pathPoints = App->GetNavigation()->FindNavPath(playerPosition, winPosition);

        for (size_t i = 0; i < pathPoints.size() - 1; i++)
        {
            if (i == 0) 
            {
                Debug::DrawLine(playerPosition, pathPoints[i], float3(1.0f, 0.0f, 0.0f));
            }
            else if (i < pathPoints.size()-1) 
            {
                Debug::DrawLine(pathPoints[i], pathPoints[i + 1], float3(1.0f, 0.0f, 0.0f));
            }
            else 
            {
                Debug::DrawLine(pathPoints[i], winPosition, float3(1.0f, 0.0f, 0.0f));
            }
        
        }
    }

}

void PlayerController::Move() 
{

    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT) {
        //float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * App->GetGameDt() * mPlayerSpeed);
        float3 newPos = (mGameObject->GetPosition() + float3(0, 0, 1) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT) {
        //float3 newPos = (mGameObject->GetPosition() + (mGameObject->GetFront() * -1) * App->GetGameDt() * mPlayerSpeed);
        float3 newPos = (mGameObject->GetPosition() + float3(0, 0, -1) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT) {
        //float3 newPos = (mGameObject->GetPosition() + (mGameObject->GetRight() * -1) * App->GetGameDt() * mPlayerSpeed);
        float3 newPos = (mGameObject->GetPosition() + float3(-1, 0, 0) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT) {
        //float3 newPos = (mGameObject->GetPosition() + mGameObject->GetRight() * App->GetGameDt() * mPlayerSpeed);
        float3 newPos = (mGameObject->GetPosition() + float3(1, 0, 0) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
    }
}

void PlayerController::Win() 
{
    if (mWinArea) {
     float3 winPosition= mWinArea->GetPosition();
     float3 playerPosition = mGameObject->GetPosition();
     
     float distance = winPosition.Distance(playerPosition);
     if (distance < 3.0f) {
         LOG("You WIN");
     }
    }

}

void PlayerController::Lose() 
{
    if (mWinArea) {
        float3 losePosition = mLoseArea->GetPosition();
        float3 playerPosition = mGameObject->GetPosition();
        if (losePosition.Distance(playerPosition) < 3.0f)
        {
            LOG("You have Lost");
        }
    }

}

void PlayerController::Rotate() 
{

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
            mGameObject->SetPosition(App->GetNavigation()->FindNearestPoint(newPos, float3(5.0f)));
        }
    }


}


