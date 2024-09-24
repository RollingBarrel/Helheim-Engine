#include "CinematicCamera.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "Gamemanager.h"
#include "ScriptComponent.h"
#include "BattleArea.h"
#include "AnimationComponent.h"
#include "ImageComponent.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "math/float3.h"
#include "MathFunc.h"

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
    MEMBER(MemberType::GAMEOBJECT, mPlayerCameraGO);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCameraGO1);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCameraGO2);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCameraGO3);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCameraGO4);
    SEPARATOR("ENEMIES");
    SEPARATOR("Idle = 1 | Chase = 2 | Charge = 3 | Attack = 4");
    MEMBER(MemberType::GAMEOBJECT, mDummyGO1);
    MEMBER(MemberType::INT, mDummy1AnimState);
    MEMBER(MemberType::GAMEOBJECT, mDummyGO2);
    MEMBER(MemberType::INT, mDummy2AnimState);
    MEMBER(MemberType::GAMEOBJECT, mDummyGO3);
    MEMBER(MemberType::INT, mDummy3AnimState);
    MEMBER(MemberType::GAMEOBJECT, mDummyGO4);
    MEMBER(MemberType::INT, mDummy4AnimState);
    SEPARATOR("BATTLE AREAS");
    MEMBER(MemberType::GAMEOBJECT, mBattleAreaGO1);
    MEMBER(MemberType::GAMEOBJECT, mBattleAreaGO2);
    MEMBER(MemberType::GAMEOBJECT, mBattleAreaGO3);
    MEMBER(MemberType::GAMEOBJECT, mBattleAreaGO4);
    SEPARATOR("PLAYER");
    MEMBER(MemberType::GAMEOBJECT, mPlayerGO);
    SEPARATOR("FADE SCREEN");
    MEMBER(MemberType::GAMEOBJECT, mFadeGO);
    MEMBER(MemberType::FLOAT3, mColor);
    MEMBER(MemberType::FLOAT, mFadeSpeed);
    SEPARATOR("HUD");
    MEMBER(MemberType::GAMEOBJECT, mHudGO);
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
    DeActivateCameras();

    if (mFadeGO)
    {
        mImage = static_cast<ImageComponent*>(mFadeGO->GetComponent(ComponentType::IMAGE));
    }

    if (mBattleAreaGO1)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO1->GetComponent(ComponentType::SCRIPT);
        mBattleArea1 = (BattleArea*)script->GetScriptInstance();

        ActivateDummy(mDummyGO1, false);
    }

    if (mBattleAreaGO2)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO2->GetComponent(ComponentType::SCRIPT);
        mBattleArea2 = (BattleArea*)script->GetScriptInstance();

        ActivateDummy(mDummyGO2, false);
    }

    if (mBattleAreaGO3)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO3->GetComponent(ComponentType::SCRIPT);
        mBattleArea3 = (BattleArea*)script->GetScriptInstance();

        ActivateDummy(mDummyGO3, false);
    }

    if (mBattleAreaGO4)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO4->GetComponent(ComponentType::SCRIPT);
        mBattleArea4 = (BattleArea*)script->GetScriptInstance();

        ActivateDummy(mDummyGO4, false);
    }
}

void CinematicCamera::Update()
{
    if (mBattleArea1)
    {
        if (mBattleArea1->GetIsAreaActive())
        {
            if (mCinematicIndex == 1)
            {
                //StartCinematic(mCinematicCameraGO1, mDummyGO1, mBattleArea1, mDummy1AnimState);
                StartCinematic(mCinematicCameraGO1, mDummyGO1, mBattleArea1, mDummy1AnimState, 67.21f, 1.50f, 7.83f, 0.00f, -90.00f, 0.00f);  
            }    
        }
    }

    if (mBattleArea2)
    {
        if (mBattleArea2->GetIsAreaActive())
        {
            if (mCinematicIndex == 2)
            {
                //StartCinematic(mCinematicCameraGO2, mDummyGO2, mBattleArea2, mDummy2AnimState);
                StartCinematic(mCinematicCameraGO1, mDummyGO2, mBattleArea2, mDummy2AnimState, 12.77f, 1.50f, -22.54f, 0.00f, -90.00f, 0.00f);
            }            
        }
    }
    
    if (mBattleArea3)
    {
        if (mBattleArea3->GetIsAreaActive())
        {
            if (mCinematicIndex == 3)
            {
                //StartCinematic(mCinematicCameraGO3, mDummyGO3, mBattleArea3, mDummy3AnimState);
                StartCinematic(mCinematicCameraGO1, mDummyGO3, mBattleArea3, mDummy3AnimState, -38.75f, 1.50f, -17.11f, 0.00f, -90.00f, 0.00f);
            }
        }
    }
    
    if (mBattleArea4)
    {
        if (mBattleArea4->GetIsAreaActive())
        {
            if (!mLevel1)
            {
                mDistanceToEnemy = mDistanceToEnemy + 4.5f; //Boss enemy is bigger than other enemies and needs more distance for the camera
            }

            if (mCinematicIndex == 4)
            {
                //StartCinematic(mCinematicCameraGO4, mDummyGO4, mBattleArea4, mDummy4AnimState);
                StartCinematic(mCinematicCameraGO1, mDummyGO4, mBattleArea4, mDummy4AnimState, -113.00f, 1.50f, 4.00f, 0.00f, -180.00f, 0.00f);
            }
        }
    }
}

/*
void CinematicCamera::StartCinematic(GameObject* camera, GameObject* dummy, BattleArea* battleArea, int animState)
{
    if ((dummy) && (camera))
    {
        if (!mStartParameters)
        {
            mStartParameters = true;
            
            mTargetPosition = ((dummy->GetWorldPosition()) - ((camera->GetFront()) * mDistanceToEnemy));
            camera->Translate(-(camera->GetFront()) * mDistanceToEnemy);

            mAnimationComponent = static_cast<AnimationComponent*>(dummy->GetComponent(ComponentType::ANIMATION));

            if (mAnimationComponent)
            {
                mAnimationComponent->SetIsPlaying(true);
            }

            InitAnimation(animState);

            //GameManager::GetInstance()->SetPaused(true, false);
                    
            mFadeOn = true;
            mHudGO->SetEnabled(false);
            ActivateBattleArea(battleArea, false);
        }

        UpdateCinematic(camera, dummy, battleArea);
    }
}
*/

void CinematicCamera::StartCinematic(GameObject* camera, GameObject* dummy, BattleArea* battleArea, int animState, 
    float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
    if ((dummy) && (camera))
    {
        if (!mStartParameters)
        {
            mStartParameters = true;

            LocateCamera(camera, posX, posY, posZ, rotX, rotY, rotZ);
            mTargetPosition = ((dummy->GetWorldPosition()) - ((camera->GetFront()) * mDistanceToEnemy));      
            camera->Translate(-(camera->GetFront()) * mDistanceToEnemy);

            mAnimationComponent = static_cast<AnimationComponent*>(dummy->GetComponent(ComponentType::ANIMATION));

            if (mAnimationComponent)
            {
                mAnimationComponent->SetIsPlaying(true);
            }

            InitAnimation(animState);

            //GameManager::GetInstance()->SetPaused(true, false);

            mFadeOn = true;
            mHudGO->SetEnabled(false);
            ActivateBattleArea(battleArea, false);
        }

        UpdateCinematic(camera, dummy, battleArea);
    }
}

void CinematicCamera::LocateCamera(GameObject* camera, float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
    float rotXRad = DegToRad(rotX);
    float rotYRad = DegToRad(rotY);
    float rotZRad = DegToRad(rotZ);

    camera->SetWorldPosition(float3(posX, posY, posZ));
    camera->SetWorldRotation(float3(rotXRad, rotYRad, rotZRad));
}

void CinematicCamera::UpdateCinematic(GameObject* camera, GameObject* dummy, BattleArea* battleArea)
{
    if (mPlayingCinematic)
    {
        if (!GameManager::GetInstance()->IsPaused())
        {
            if (mTravelling)
            {
                if (HandleFadeIn(camera))
                {
                    ActivateDummy(dummy, true);
                    HandleCameraMovement(camera);
                }

                if (mTimer.Delay(mAnimationTime))
                {
                    if (mAnimationComponent)
                    {
                        mAnimationComponent->SetIsPlaying(false);
                    }

                    mTravelling = false;
                    mFadeOn = true;
                }
            }
            else
            {
                if (HandleFadeOut(dummy, battleArea))
                {
                    mTravelling = true;
                    mPlayingCinematic = false;
                }
            }

            HandleEscape(dummy, battleArea);
        }
    }
    else
    {
        mCinematicIndex++;
        mStartParameters = false;
        mPlayingCinematic = true;
        mHudGO->SetEnabled(true);
        ActivateBattleArea(battleArea, true);
    }
}

bool CinematicCamera::HandleFadeIn(GameObject* camera)
{
    if (mFadeOn)
    {
        if (Fade(true))
        {
            if (mPlayerCameraGO)
            {
                ActivateCamera(camera, true);
                mPlayerCameraGO->SetEnabled(false);
            }

            mCinematicCamera = (CameraComponent*)camera->GetComponent(ComponentType::CAMERA);
            mActiveCinematicCamera = true;
            App->GetCamera()->ActivateFirstCamera();

            mFadeOn = false;
        }

        return false;
    }
    else
    {
        Fade(false);
        return true;
    }
}

void CinematicCamera::HandleCameraMovement(GameObject* camera)
{
    float deltaTime = App->GetDt();

    if (!mMoveCompleted)
    {
        float3 currentPosition = camera->GetWorldPosition();
        float3 direction = mTargetPosition - currentPosition;
        float distance = direction.Length();
        direction.Normalize();

        float step = mSpeedFactor * deltaTime;

        if (step >= distance)
        {
            camera->SetWorldPosition(mTargetPosition);
            mMoveCompleted = true;
        }
        else
        {
            float3 newPosition = Lerp(currentPosition, mTargetPosition, mSpeedFactor * deltaTime);

            if (newPosition.y < currentPosition.y)
            {
                newPosition.y = currentPosition.y;
            }

            camera->SetWorldPosition(newPosition);
        }
    }
}

bool CinematicCamera::HandleFadeOut(GameObject* dummy, BattleArea* battleArea)
{
    if (mFadeOn)
    {
        if (Fade(true))
        {
            EndCinematic(dummy, battleArea);
            mFadeOn = false;
        }

        return false;
    }
    else
    {
        if (Fade(false))
        {
            return true;
        }
    }
}

void CinematicCamera::HandleEscape(GameObject* dummy, BattleArea* battleArea)
{
    if (!mEscape)
    {
        if ((App->GetInput()->GetKey(Keys::Keys_X) == KeyState::KEY_REPEAT) ||
            (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_X) == ButtonState::BUTTON_REPEAT))
        {
            mEscape = true;
            mFadeStart = false;
            mTravelling = false;
            mTimer.Reset();
            EndCinematic(dummy, battleArea);
        }
    }

    if (mEscape)
    {
        if (Fade(false))
        {
            mEscape = false;
            mTravelling = true;
            mPlayingCinematic = false;
        }
    }
}

void CinematicCamera::EndCinematic(GameObject* dummy, BattleArea* battleArea)
{
    if (mActiveCinematicCamera)
    {
        mActiveCinematicCamera = false;
        App->GetCamera()->RemoveEnabledCamera(mCinematicCamera);
        DeActivateCameras();
        mPlayerCameraGO->SetEnabled(true);
        App->GetCamera()->ActivateFirstCamera();
    }
    
    //GameManager::GetInstance()->SetPaused(false, false);

    ActivateDummy(dummy, false);

    if (mLevel1)
    {
        if (mPlayerGO)
        {
            //Locates the player in a correct position behind the doors
            float3 position = mPlayerGO->GetWorldPosition();
            position.x -= 2.6f;
            mPlayerGO->SetWorldPosition(position);
        }
    }
}

void CinematicCamera::DeActivateCameras()
{
    ActivateCamera(mCinematicCameraGO1, false);
    ActivateCamera(mCinematicCameraGO2, false);
    ActivateCamera(mCinematicCameraGO3, false);
    ActivateCamera(mCinematicCameraGO4, false);
}

void CinematicCamera::ActivateCamera(GameObject* cinematicCamera, bool state)
{
    if (cinematicCamera)
    {
        cinematicCamera->SetEnabled(state);
    }
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

        default: trigger = "tIdle"; break;
    }

    if (mAnimationComponent)
    {
        mAnimationComponent->ResetAnimationComponent();
        mAnimationComponent->SendTrigger(trigger, 0.0f);
    }
}

bool CinematicCamera::Fade(bool fadeOut)
{
    if (mFadeGO)
    {
        mImage->SetColor(mColor);

        if (fadeOut)
        {
            if (!mFadeStart)
            {
                mFadeStart = true;
                mCounter = 0.0f;
            }
            
            if (mCounter < 1.0f)
            {
                mCounter += mFadeSpeed;
                mImage->SetAlpha(mCounter);

                return false;
            }

            else return true;
        }
        else
        {
            if (!mFadeStart)
            {
                mFadeStart = true;
                mCounter = 1.0f;
            }
            
            if (mCounter > 0.0f)
            {
                mCounter -= mFadeSpeed;
                mImage->SetAlpha(mCounter);

                return false;
            }
            else return true;      
        }
    }
}

void CinematicCamera::ActivateBattleArea(BattleArea* battleArea, bool state)
{
    if (battleArea)
    {
        battleArea->SetSpawnEnemies(state);
    }
}

void CinematicCamera::ActivateDummy(GameObject* dummy, bool state)
{
    if (dummy)
    {
        dummy->SetEnabled(state);
    }  
}



/*
    if (mBattleArea4)
    {
        if (mBattleArea4->GetIsAreaActive())
        {
            if (mLevel1) //In Level 1 you need a dummy, in Level 2 you need to use the Final Boss asset
            {
                mEnemyGO4->SetEnabled(false);
            }
            else
            {
                mAnimationComponent->SetIsPlaying(true);
            }
        }
    }
}
*/