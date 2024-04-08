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
#include "Geometry/Ray.h"
#include <unordered_map>

/*extern "C" SCRIPTING_API Script * CreatePlayerController(GameObject * owner, std::vector<ScriptVariable*>&data)
{

    PlayerController* script = new PlayerController(owner);

    data.push_back(new ScriptVariable("mPlayerSpeed", MemberType::FLOAT, &script->mPlayerSpeed));
    data.push_back(new ScriptVariable("mPlayerRotationSpeed", MemberType::FLOAT, &script->mPlayerRotationSpeed));
    data.push_back(new ScriptVariable("mWinArea", MemberType::GAMEOBJECT, &script->mWinArea));
    data.push_back(new ScriptVariable("mLoseArea", MemberType::GAMEOBJECT, &script->mLoseArea));
    data.push_back(new ScriptVariable("mAnimationComponentHolder", MemberType::GAMEOBJECT, &script->mAnimationComponentHolder));

    data.push_back(new ScriptVariable("mDashSpeed", MemberType::FLOAT, &script->mDashSpeed));
    data.push_back(new ScriptVariable("mDashLenght", MemberType::FLOAT, &script->mDashLenght));
    data.push_back(new ScriptVariable("mDashCoolDown", MemberType::FLOAT, &script->mDashCoolDown));

    return script;
}*/

/*std::vector<Member> PlayerController::Serialize()
{

    offsetof(PlayerController, mPlayerSpeed);

    std::vector<Member> members;

    members.push_back(Member("mPlayerSpeed", MemberType::FLOAT, offsetof(PlayerController, mPlayerSpeed)));
    members.push_back(Member("mPlayerRotationSpeed", MemberType::FLOAT, offsetof(PlayerController, mPlayerRotationSpeed)));
    members.push_back(Member("mWinArea", MemberType::GAMEOBJECT, offsetof(PlayerController, mWinArea)));
    members.push_back(Member("mTest", MemberType::FLOAT, offsetof(PlayerController, mTest)));

    return members;
}*/
extern "C" SCRIPTING_API Script * CreatePlayerController(GameObject * owner, std::vector<Member*>&data)
{
    
    PlayerController* script = new PlayerController(owner);
    return script;
}

void PlayerController::Serialize()
{


    CLASS(PlayerController);
    MEMBER(MemberType::FLOAT, mPlayerSpeed);

  
}



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
    Lose();
    Dash();

    if (mAnimationComponent) {
        mAnimationComponent->OnUpdate();
    }

    std::map<float, GameObject*> hits;

    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN) {
        
        Ray ray;
        ray.pos = mGameObject->GetPosition();
        ray.dir = mGameObject->GetFront();
        float distance = 100;
        hits = Physics::Raycast(&ray);
        Debug::DrawLine(ray.pos, ray.dir*distance, float3(1.0f, 0.0f, 0.0f));
        for (const auto& hit : hits) {
            LOG("Object %s has been hit at distance: %f", hit.second->GetName().c_str(), hit.first);
        }
    }
    mTest++;
    LOG("Prueba HotReload %f", mTest);

    LOG("Adri cabron");
}

void PlayerController::Move() {
    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT) {
        //float3 newPos = (mGameObject->GetPosition() + mGameObject->GetFront() * App->GetGameDt() * mPlayerSpeed);
        float3 newPos = (mGameObject->GetPosition() + float3(0, 0, 1) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT) {
        //float3 newPos = (mGameObject->GetPosition() + (mGameObject->GetFront() * -1) * App->GetGameDt() * mPlayerSpeed);
        float3 newPos = (mGameObject->GetPosition() + float3(0, 0, -1) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT) {
        //float3 newPos = (mGameObject->GetPosition() + (mGameObject->GetRight() * -1) * App->GetGameDt() * mPlayerSpeed);
        float3 newPos = (mGameObject->GetPosition() + float3(-1, 0, 0) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));
    }
    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT) {
        //float3 newPos = (mGameObject->GetPosition() + mGameObject->GetRight() * App->GetGameDt() * mPlayerSpeed);
        float3 newPos = (mGameObject->GetPosition() + float3(1, 0, 0) * App->GetGameDt() * mPlayerSpeed);
        mGameObject->SetPosition(mNavMeshControl->FindNearestPoint(newPos, float3(5.0f)));
    }
}

void PlayerController::Win() {
    if (mWinArea) {
     float3 winPosition= mWinArea->GetPosition();
     float3 playerPosition = mGameObject->GetPosition();
     
     float distance = winPosition.Distance(playerPosition);
     if (distance < 3.0f) {
         LOG("You WIN");
     }
    }

}

void PlayerController::Lose() {
    if (mWinArea) {
        float3 losePosition = mLoseArea->GetPosition();
        float3 playerPosition = mGameObject->GetPosition();
        if (losePosition.Distance(playerPosition) < 3.0f)
        {
            LOG("You have Lost");
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




