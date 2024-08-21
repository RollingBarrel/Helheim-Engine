#include "CinematicCamera.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "Gamemanager.h"
#include "ScriptComponent.h"
#include "BattleArea.h"
#include "AnimationComponent.h"
#include "ImageComponent.h"

CREATE(CinematicCamera)
{
    CLASS(owner);
    SEPARATOR("LEVEL");
    MEMBER(MemberType::BOOL, mLevel1);
    SEPARATOR("PARAMS");
    MEMBER(MemberType::FLOAT, mYawAngle);
    MEMBER(MemberType::FLOAT, mPitchAngle);
    MEMBER(MemberType::FLOAT, mDistanceToEnemy);
    MEMBER(MemberType::FLOAT, mSpeedFactor);
    MEMBER(MemberType::FLOAT, mAnimationTime);
    SEPARATOR("CAMERAS");
    MEMBER(MemberType::GAMEOBJECT, mCinematicCamera1);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCamera2);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCamera3);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCamera4);
    SEPARATOR("ENEMIES");
    SEPARATOR("Idle = 1 | Chase = 2 | Charge = 3 | Attack = 4");
    MEMBER(MemberType::GAMEOBJECT, mEnemy1);
    MEMBER(MemberType::INT, mEnemy1_AnimState);
    MEMBER(MemberType::GAMEOBJECT, mEnemy2);
    MEMBER(MemberType::INT, mEnemy2_AnimState);
    MEMBER(MemberType::GAMEOBJECT, mEnemy3);
    MEMBER(MemberType::INT, mEnemy3_AnimState);
    MEMBER(MemberType::GAMEOBJECT, mEnemy4);
    MEMBER(MemberType::INT, mEnemy4_AnimState);
    SEPARATOR("BATTLE AREAS");
    MEMBER(MemberType::GAMEOBJECT, mBattleArea1);
    MEMBER(MemberType::GAMEOBJECT, mBattleArea2);
    MEMBER(MemberType::GAMEOBJECT, mBattleArea3);
    MEMBER(MemberType::GAMEOBJECT, mBattleArea4);
    SEPARATOR("FADE SCREEN");
    MEMBER(MemberType::GAMEOBJECT, mFade);
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

    if (mFade)
    {
        mImage = static_cast<ImageComponent*>(mFade->GetComponent(ComponentType::IMAGE));
    }

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
            StartCinematic(mCinematicCamera1, mEnemy1, mEnemy1_AnimState);
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

            StartCinematic(mCinematicCamera2, mEnemy2, mEnemy2_AnimState);
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

            StartCinematic(mCinematicCamera3, mEnemy3, mEnemy3_AnimState);
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

            if (!mLevel1)
            {
                mDistanceToEnemy = mDistanceToEnemy + 4.5f; //Boss enemy is bigger than other enemies and needs more distance for the camera
            }

            StartCinematic(mCinematicCamera4, mEnemy4, mEnemy4_AnimState);
        }
    }
}

void CinematicCamera::StartCinematic(GameObject* camera, GameObject* target, int animState)
{
    if ((target) && (camera))
    {
        if (mStartParameters == false)
        {
            mStartParameters = true;

            ActivateCamera(camera);

            mTargetPosition = ((target->GetWorldPosition()) - ((camera->GetFront()) * mDistanceToEnemy));
            camera->Translate(-(camera->GetFront()) * mDistanceToEnemy);

            mAnimationComponent = static_cast<AnimationComponent*>(target->GetComponent(ComponentType::ANIMATION));

            if (mAnimationComponent)
            {
                mAnimationComponent->SetIsPlaying(true);
            }

            InitAnimation(animState);
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

                    if (newPosition.y < currentPosition.y) { // Adjust the threshold as needed
                        newPosition.y = currentPosition.y; // Ensure the camera stays above a certain height
                    }

                    camera->SetWorldPosition(newPosition);
                }
            }
            
            if (mTimer.Delay(mAnimationTime))
            {
                if (mAnimationComponent)
                {
                    mAnimationComponent->SetIsPlaying(false);
                }

                mPlayingCinematic = false;
            }
        }    
        else
        {
            if(Fade())
            {
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
                        if (mLevel1) //In Level 1 you need a dummy, in Level 2 you need to use the Final Boss asset
                        {
                            mEnemy4->SetEnabled(false);
                        }
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

void CinematicCamera::InitAnimation(int animState)
{
    std::string trigger;

    switch (animState)
    {
        case 1: trigger = "tIdle"; break;
        case 2: trigger = "tChase"; break;
        case 3: trigger = "tCharge"; break;
        case 4: trigger = "tAttack"; break;
        //case 5: trigger = "tDeath"; break;

        default: trigger = "tIdle"; break;
    }

    if (mAnimationComponent)
    {
        mAnimationComponent->OnReset();
        mAnimationComponent->SendTrigger(trigger, 0.0f);
    }
}

//Fade (float3 color, bool fadeOut)
bool CinematicCamera::Fade()
{
    if (mFade)
    {
        if (mCounter < 1.0f)
        {
            mFade->SetEnabled(true);

            mCounter += 0.02f;
            mImage->SetAlpha(mCounter);
            //mImage->SetColor(float3(1.0f, 1.0f, 1.0f));

            return false;
        }
        else
        {
            mFade->SetEnabled(false);
            return true;
            /*
            if (!mTimer.Delay(2.0f))
            {
                mFade->SetEnabled(false);
            } 
            */

            /*
            if (mCounter > 0.0f)
            {
                mCounter -= 0.02f;
                mImage->SetAlpha(mCounter);

                //mFade->SetEnabled(false);
            }
            */
        }
    }
}