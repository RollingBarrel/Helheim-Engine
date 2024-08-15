#include "CinematicCamera.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "Gamemanager.h"
#include "ScriptComponent.h"
#include "BattleArea.h"
#include "AnimationComponent.h"

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
    MEMBER(MemberType::GAMEOBJECT, mBattleArea4);
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

    if (mBattleArea4)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleArea4->GetComponent(ComponentType::SCRIPT);
        mBArea4 = (BattleArea*)script->GetScriptInstance();
    }
}

void CinematicCamera::Update()
{
    if (mBattleArea1)
    {
        if (mBArea1->IsAreaActive())
        {
            StartCinematic(mEnemy1, mCinematicCamera1);
        }
    }
    
    if (mBattleArea2)
    {
        if (mBArea2->IsAreaActive())
        {
            if (!mCinematicStarted)
            {
                mCinematicStarted = true;
                mStartParameters = false;
                mPlayingCinematic = true;
            }

            StartCinematic(mEnemy2, mCinematicCamera2);
        }
    }
    
    if (mBattleArea3)
    {
        if (mBArea3->IsAreaActive())
        {
            if (mCinematicStarted)
            {
                mCinematicStarted = false;
                mStartParameters = false;
                mPlayingCinematic = true;
            }

            StartCinematic(mEnemy3, mCinematicCamera3);
        }
    }
    
    if (mBattleArea4)
    {
        if (mBArea4->IsAreaActive())
        {
            if (!mCinematicStarted)
            {
                mCinematicStarted = true;
                mStartParameters = false;
                mPlayingCinematic = true;
            }

            StartCinematic(mEnemy4, mCinematicCamera4);
        }
    }
}

void CinematicCamera::StartCinematic(GameObject* target, GameObject* camera)
{
    if ((target) && (camera))
    {
        if (mStartParameters == false)
        {
            mStartParameters = true;

            ActivateCamera(camera);

            mTargetPosition = ((target->GetWorldPosition()) - ((camera->GetFront()) * mDistanceToEnemy));
            camera->Translate(-(camera->GetFront()) * mDistanceToEnemy);

            //**************************************************************

            mAnimationComponent = static_cast<AnimationComponent*>(target->GetComponent(ComponentType::ANIMATION));
            
            if (mAnimationComponent)
            {
                mAnimationComponent->SetIsPlaying(true);
            }

            //************************************

            //InitAnimation();

            //**************************************************************
        }

        if (mPlayingCinematic)
        {
            if (mMainCamera)
            {
                App->GetCamera()->RemoveEnabledCamera(mMainCamera);
                GameManager::GetInstance()->SetPaused(true, false);
            }
            
            mCinematicCamera = (CameraComponent*)camera->GetComponent(ComponentType::CAMERA);
            App->GetCamera()->ActivateFirstCamera();

            //***********************************************************
           
            if (mAnimationComponent) mAnimationComponent->SendTrigger("tIdle", 0.2f);
            //if (mAnimationComponent) mAnimationComponent->SendTrigger("tChase", 0.2f);
            //if (mAnimationComponent) mAnimationComponent->SendTrigger("tCharge", 0.2f);
            //if (mAnimationComponent) mAnimationComponent->SendTrigger("tAttack", 0.2f);
            //if (mAnimationComponent) mAnimationComponent->SendTrigger("tDeath", 0.2f);

            //***********************************************************

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
                App->GetCamera()->ActivateFirstCamera();

                GameManager::GetInstance()->SetPaused(false, false);

                if (mBattleArea1)
                {
                    if (mBArea1->IsAreaActive())
                    {
                        mEnemy1->SetEnabled(false);
                    }
                }
                
                if (mBattleArea2)
                {
                    if (mBArea2->IsAreaActive())
                    {
                        mEnemy2->SetEnabled(false);
                    }
                }
                
                if (mBattleArea3)
                {
                    if (mBArea3->IsAreaActive())
                    {
                        mEnemy3->SetEnabled(false);
                    }
                }
                
                if (mBattleArea4)
                {
                    if (mBArea4->IsAreaActive())
                    {
                        mEnemy4->SetEnabled(false);
                    }
                }     
            }
        }
    }
}

void CinematicCamera::ActivateCamera(GameObject* cinematicCamera)
{
    if (mCinematicCamera1)
    {
        if (cinematicCamera == mCinematicCamera1)
        {
            mCinematicCamera1->SetEnabled(true);
        }
        else
        {
            mCinematicCamera1->SetEnabled(false);
        }
    }

    if (mCinematicCamera2)
    {
        if (cinematicCamera == mCinematicCamera2)
        {
            mCinematicCamera2->SetEnabled(true);
        }
        else
        {
            mCinematicCamera2->SetEnabled(false);
        }
    }

    if (mCinematicCamera3)
    {
        if (cinematicCamera == mCinematicCamera3)
        {
            mCinematicCamera3->SetEnabled(true);
        }
        else
        {
            mCinematicCamera3->SetEnabled(false);
        }
    }

    if (mCinematicCamera4)
    {
        if (cinematicCamera == mCinematicCamera4)
        {
            mCinematicCamera4->SetEnabled(true);
        }
        else
        {
            mCinematicCamera4->SetEnabled(false);
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

/*
void CinematicCamera::InitAnimation()
{
    if (mAnimationComponent)
    {
        mAnimationComponent->OnReset();
        mAnimationComponent->SendTrigger("tIdle", 0.0f);
    }
}
*/