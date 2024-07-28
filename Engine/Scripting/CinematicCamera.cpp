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
        StartCinematic(mEnemy1, 67.35f, 1.50f, 5.65f, 0.00f, -45.50f, 0.00f);
    }

    if (mBArea2->IsAreaActive())
    {
        if (!mCinematicStarted)
        {
            mCinematicStarted = true;
            mStartParameters = false;
            mPlayingCinematic = true;
        }

        StartCinematic(mEnemy2, 6.57f, 1.50f, -22.66f, 0.00f, -45.50f, 0.00f);
    }

    if (mBArea3->IsAreaActive())
    {
        if (mCinematicStarted)
        {
            mCinematicStarted = false;
            mStartParameters = false;
            mPlayingCinematic = true;
        }

        StartCinematic(mEnemy3, -113.00f, 1.50f, 0.33f, 0.00f, 330.00f, 0.00f);
    }
}

void CinematicCamera::StartCinematic(GameObject* target, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
    if (mStartParameters == false)
    {
        mStartParameters = true;

        mTargetPosition = ((target->GetWorldPosition()) - ((mGameObject->GetFront()) * mDistanceToEnemy));
        mGameObject->SetWorldPosition(float3(posX, posY, posZ));
        mGameObject->SetWorldRotation(float3(rotX, rotY, rotZ));
        mGameObject->Translate(-(mGameObject->GetFront()) * mDistanceToEnemy);
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
            mCurrentCamera = App->GetCamera()->GetCurrentCamera();
            mCinematicCamera = const_cast<CameraComponent*>(mCurrentCamera);
        }
        else
        {
            App->GetCamera()->AddEnabledCamera(mCinematicCamera);
            App->GetCamera()->ActivateFirstCamera();
        }

        float deltaTime = App->GetDt();

        if (!mMoveCompleted)
        {
            float3 currentPosition = mGameObject->GetWorldPosition();
            float3 direction = mTargetPosition - currentPosition;
            float distance = direction.Length();
            direction.Normalize();

            float step = mSpeedFactor * deltaTime;

            //Pause the game
            GameManager::GetInstance()->SetPaused(true, false);

            if (step >= distance)
            {
                mGameObject->SetWorldPosition(mTargetPosition);
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
                mGameObject->SetWorldPosition(newPosition);
            }
        }

        if (Delay(mAnimationTime))
        {
            mPlayingCinematic = false;
            App->GetCamera()->RemoveEnabledCamera(mCinematicCamera);
            App->GetCamera()->AddEnabledCamera(mMainCamera);
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