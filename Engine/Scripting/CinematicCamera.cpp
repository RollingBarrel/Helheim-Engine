#include "CinematicCamera.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "Gamemanager.h"
#include "ScriptComponent.h"
#include "BattleArea.h"

CREATE(CinematicCamera)
{
    CLASS(owner);
    MEMBER(MemberType::FLOAT, mYawAngle);
    MEMBER(MemberType::FLOAT, mPitchAngle);
    MEMBER(MemberType::FLOAT, mDistanceToEnemy);
    MEMBER(MemberType::FLOAT, mSpeedFactor);
    MEMBER(MemberType::FLOAT, mAnimationTime);

    MEMBER(MemberType::GAMEOBJECT, mCinematicCamera1);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCamera2);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCamera3);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCamera4);

    MEMBER(MemberType::GAMEOBJECT, mEnemy1);
    MEMBER(MemberType::GAMEOBJECT, mEnemy2);
    MEMBER(MemberType::GAMEOBJECT, mEnemy3);
    MEMBER(MemberType::GAMEOBJECT, mEnemy4);

    MEMBER(MemberType::GAMEOBJECT, mBattleArea1);
    MEMBER(MemberType::GAMEOBJECT, mBattleArea2);
    MEMBER(MemberType::GAMEOBJECT, mBattleArea3);
    END_CREATE;
}

CinematicCamera::CinematicCamera(GameObject* owner) : Script(owner)
{
}

void CinematicCamera::Awake()
{
}

void CinematicCamera::Start()
{
    mCurrentCamera = App->GetCamera()->GetCurrentCamera();
    mMainCamera = const_cast<CameraComponent*>(mCurrentCamera);

    if (mBattleArea1)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleArea1->GetComponent(ComponentType::SCRIPT);
        mBArea1 = (BattleArea*)script->GetScriptInstance();
    }

    if (mBattleArea2)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleArea2->GetComponent(ComponentType::SCRIPT);
        mBArea2 = (BattleArea*)script->GetScriptInstance();
    }

    if (mBattleArea3)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleArea3->GetComponent(ComponentType::SCRIPT);
        mBArea3 = (BattleArea*)script->GetScriptInstance();
    }
}

void CinematicCamera::Update()
{
    if (mBArea1->IsAreaActive())
    {
        //********************************************

        if (mCinematicCamera2)
        {
            mCinematicCamera2->SetEnabled(false);
        }

        if (mCinematicCamera3)
        {
            mCinematicCamera3->SetEnabled(false);
        }

        if (mCinematicCamera4)
        {
            mCinematicCamera4->SetEnabled(false);
        }

        //********************************************

        StartCinematic(mEnemy1, mCinematicCamera1);
    }

    if (mBArea2->IsAreaActive())
    {
        if (!mCinematicStarted)
        {
            mCinematicStarted = true;
            mStartParameters = false;
            mPlayingCinematic = true;
        }

        //********************************************

        if (mCinematicCamera1)
        {
            mCinematicCamera1->SetEnabled(false);
        }

        if (mCinematicCamera2)
        {
            mCinematicCamera2->SetEnabled(true);
        }

        if (mCinematicCamera3)
        {
            mCinematicCamera3->SetEnabled(false);
        }

        if (mCinematicCamera4)
        {
            mCinematicCamera4->SetEnabled(false);
        }

        //********************************************

        StartCinematic(mEnemy2, mCinematicCamera2);
    }

    if (mBArea3->IsAreaActive())
    {
        if (mCinematicStarted)
        {
            mCinematicStarted = false;
            mStartParameters = false;
            mPlayingCinematic = true;
        }

        //********************************************

        if (mCinematicCamera1)
        {
            mCinematicCamera1->SetEnabled(false);
        }

        if (mCinematicCamera2)
        {
            mCinematicCamera2->SetEnabled(false);
        }

        if (mCinematicCamera3)
        {
            mCinematicCamera3->SetEnabled(true);
        }

        if (mCinematicCamera4)
        {
            mCinematicCamera4->SetEnabled(false);
        }

        //********************************************

        StartCinematic(mEnemy3, mCinematicCamera3);
    }
}

void CinematicCamera::StartCinematic(GameObject* target, GameObject* camera)
{
    if (mStartParameters == false)
    {
        mStartParameters = true;

        mTargetPosition = ((target->GetWorldPosition()) - ((camera->GetFront()) * mDistanceToEnemy));
        //mGameObject->SetWorldPosition(float3(posX, posY, posZ));
        //mGameObject->SetWorldRotation(float3(rotX, rotY, rotZ));
        camera->Translate(-(camera->GetFront()) * mDistanceToEnemy);
    }

    if (mPlayingCinematic)
    {
        if (mMainCamera)
        {
            App->GetCamera()->RemoveEnabledCamera(mMainCamera);
            GameManager::GetInstance()->SetPaused(true, false);
        }

        if (!mCinematicCamera)
        {
            //mCurrentCamera = App->GetCamera()->GetCurrentCamera();
            //mCinematicCamera = const_cast<CameraComponent*>(mCurrentCamera);

            mCinematicCamera = (CameraComponent*)camera->GetComponent(ComponentType::CAMERA);
            //App->GetCamera()->AddEnabledCamera(mCinematicCamera);
        }
        else
        {
            App->GetCamera()->AddEnabledCamera(mCinematicCamera);
            //App->GetCamera()->ActivateFirstCamera();
        }
        
        float deltaTime = App->GetDt();

        if (!mMoveCompleted)
        {
            float3 currentPosition = camera->GetWorldPosition();
            float3 direction = mTargetPosition - currentPosition;
            float distance = direction.Length();
            direction.Normalize();

            float step = mSpeedFactor * deltaTime;

            //Pause the game
            GameManager::GetInstance()->SetPaused(true, false);

            if (step >= distance)
            {
                camera->SetWorldPosition(mTargetPosition);
                mMoveCompleted = true;

                //Restore the game
                GameManager::GetInstance()->SetPaused(false, false);
            }
            else
            {
                float3 newPosition = lerp(currentPosition, mTargetPosition, mSpeedFactor * deltaTime);
                if (newPosition.y < 1.5f) { // Adjust the threshold as needed
                    newPosition.y = 1.5f; // Ensure the camera stays above a certain height
                }
                camera->SetWorldPosition(newPosition);
            }
        }

        if (Delay(mAnimationTime))
        {
            mPlayingCinematic = false;
            App->GetCamera()->RemoveEnabledCamera(mCinematicCamera);
            App->GetCamera()->AddEnabledCamera(mMainCamera);
            //App->GetCamera()->ActivateFirstCamera();

            GameManager::GetInstance()->SetPaused(false, false);

            if (mBArea1->IsAreaActive())
            {
                mEnemy1->SetEnabled(false);
            }
            else if (mBArea2->IsAreaActive())
            {
                mEnemy2->SetEnabled(false);
            }
            else if (mBArea3->IsAreaActive())
            {
                mEnemy3->SetEnabled(false);
            }
        }
    }
}

float3 CinematicCamera::lerp(const float3& start, const float3& end, float t)
{
    return float3{
        start.x + t * (end.x - start.x),
        start.y + t * (end.y - start.y),
        start.z + t * (end.z - start.z)
    };
}

bool CinematicCamera::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}