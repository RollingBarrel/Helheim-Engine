#include "CinematicCamera.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "Gamemanager.h"
#include "ScriptComponent.h"
#include "BattleArea.h"
#include "AnimationComponent.h"
#include "ImageComponent.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "MathFunc.h"
#include "PlayerController.h"

CREATE(CinematicCamera)
{
    CLASS(owner);
    SEPARATOR("PARAMS");
    MEMBER(MemberType::FLOAT, mYawAngle);
    MEMBER(MemberType::FLOAT, mPitchAngle);
    MEMBER(MemberType::FLOAT, mDistanceToEnemy);
    MEMBER(MemberType::FLOAT, mSpeedFactor);
    MEMBER(MemberType::FLOAT, mAnimationTime);
    SEPARATOR("CAMERAS");
    MEMBER(MemberType::GAMEOBJECT, mPlayerCameraGO);
    MEMBER(MemberType::GAMEOBJECT, mCinematicCameraGO);
    SEPARATOR("CAMERA OBJECTS");
    MEMBER(MemberType::GAMEOBJECT, mCameraObjectGO1);
    MEMBER(MemberType::GAMEOBJECT, mCameraObjectGO2);
    MEMBER(MemberType::GAMEOBJECT, mCameraObjectGO3);
    MEMBER(MemberType::GAMEOBJECT, mCameraObjectGO4);
    SEPARATOR("ENEMIES");
    SEPARATOR("Idle 1 | Chase 2 | Charge 3 | Attack 4 | WakeUp 5");
    MEMBER(MemberType::GAMEOBJECT, mEnemyGO1);
    MEMBER(MemberType::INT, mEnemyAnimState1);
    MEMBER(MemberType::GAMEOBJECT, mEnemyGO2);
    MEMBER(MemberType::INT, mEnemyAnimState2);
    MEMBER(MemberType::GAMEOBJECT, mEnemyGO3);
    MEMBER(MemberType::INT, mEnemyAnimState3);    
    MEMBER(MemberType::GAMEOBJECT, mEnemyGO4);
    MEMBER(MemberType::INT, mEnemyAnimState4);
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

CinematicCamera::CinematicCamera(GameObject* owner) : Script(owner) {}

CinematicCamera::~CinematicCamera() {}

void CinematicCamera::Awake() {}

void CinematicCamera::Start()
{
    ActivateCamera(false);

    if (mFadeGO)
    {
        mImage = static_cast<ImageComponent*>(mFadeGO->GetComponent(ComponentType::IMAGE));
    }
    
    if (mPlayerGO)
    {
        ScriptComponent* playerScript = (ScriptComponent*)mPlayerGO->GetComponent(ComponentType::SCRIPT);
        mPlayerController = (PlayerController*)playerScript->GetScriptInstance();
    }

    if (mBattleAreaGO1)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO1->GetComponent(ComponentType::SCRIPT);
        mBattleArea1 = (BattleArea*)script->GetScriptInstance();

        if (mEnemyGO1->GetName() != "FinalBoss")
        {
            ActivateDummy(mEnemyGO1, false);
        }
    }

    if (mBattleAreaGO2)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO2->GetComponent(ComponentType::SCRIPT);
        mBattleArea2 = (BattleArea*)script->GetScriptInstance();

        ActivateDummy(mEnemyGO2, false);
    }

    if (mBattleAreaGO3)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO3->GetComponent(ComponentType::SCRIPT);
        mBattleArea3 = (BattleArea*)script->GetScriptInstance();

        ActivateDummy(mEnemyGO3, false);
    }

    if (mBattleAreaGO4)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO4->GetComponent(ComponentType::SCRIPT);
        mBattleArea4 = (BattleArea*)script->GetScriptInstance();

        ActivateDummy(mEnemyGO4, false);
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
                StartCinematic(mCameraObjectGO1, mEnemyGO1, mBattleArea1, mEnemyAnimState1);
            }    
        }
    }

    if (mBattleArea2)
    {
        if (mBattleArea2->GetIsAreaActive())
        {
            if (mCinematicIndex == 2)
            {
                StartCinematic(mCameraObjectGO2, mEnemyGO2, mBattleArea2, mEnemyAnimState2);
            }            
        }
    }
    
    if (mBattleArea3)
    {
        if (mBattleArea3->GetIsAreaActive())
        {
            if (mCinematicIndex == 3)
            {
                StartCinematic(mCameraObjectGO3, mEnemyGO3, mBattleArea3, mEnemyAnimState3);
            }
        }
    }
    
    if (mBattleArea4)
    {
        if (mBattleArea4->GetIsAreaActive())
        {
            if (mCinematicIndex == 4)
            {
                StartCinematic(mCameraObjectGO4, mEnemyGO4, mBattleArea4, mEnemyAnimState4);
            }
        }
    }
}

void CinematicCamera::StartCinematic(GameObject* cameraObject, GameObject* dummy, BattleArea* battleArea, int animState)
{
    if ((dummy) && (mCinematicCameraGO))
    {
        if (!mStartParameters)
        {
            mStartParameters = true;

            LocateCamera(cameraObject);
            mTargetPosition = ((dummy->GetWorldPosition()) - ((mCinematicCameraGO->GetFront()) * mDistanceToEnemy));
            mCinematicCameraGO->Translate(-(mCinematicCameraGO->GetFront()) * mDistanceToEnemy);

            mAnimationComponent = static_cast<AnimationComponent*>(dummy->GetComponent(ComponentType::ANIMATION));

            if (mAnimationComponent)
            {
                mAnimationComponent->SetIsPlaying(true);
            }

            InitAnimation(animState);

            mPlayingCinematic = true;
            mTravelling = true;
            mFadeOn = true;

            if (mHudGO)
            {
                mHudGO->SetEnabled(false);
            }
            
            if (mPlayerController)
            {
                mPlayerController->EnableLaser(false);
            }

            ActivateBattleArea(battleArea, false);
        }

        UpdateCinematic(dummy, battleArea);
    }
}

void CinematicCamera::UpdateCinematic(GameObject* dummy, BattleArea* battleArea)
{
    if ((!GameManager::GetInstance()->IsPaused()) && (!mEscape))
    {
        if (mPlayingCinematic)
        {
            if (mTravelling)
            {
                if (HandleFadeIn())
                {
                    if (dummy->GetName() != "FinalBoss")
                    {
                        ActivateDummy(dummy, true);
                    }
                    
                    HandleCameraMovement();
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
                if (HandleFadeOut(dummy))
                {
                    mPlayingCinematic = false;
                }
            }
        }
        else
        {
            mCinematicIndex++;
            mStartParameters = false;

            if (mHudGO)
            {
                mHudGO->SetEnabled(true);
            }
            
            if (mPlayerController)
            {
                mPlayerController->EnableLaser(true);
            }

            ActivateBattleArea(battleArea, true);
        }

        HandleEscape(dummy);
    }

    if (mEscape)
    {
        if (Fade(false))
        {
            mEscape = false;
            mPlayingCinematic = false;
        }

        return;
    }
}

void CinematicCamera::LocateCamera(GameObject* cameraObject)
{
    if (cameraObject)
    {
        float3 objectPosition = cameraObject->GetWorldPosition();
        Quat objectRotation = cameraObject->GetWorldRotation();

        mCinematicCameraGO->SetWorldPosition(objectPosition);
        mCinematicCameraGO->SetWorldRotation(objectRotation);
    }
}

bool CinematicCamera::HandleFadeIn()
{
    if (mFadeOn)
    {
        if (Fade(true))
        {
            if (mPlayerCameraGO)
            {
                ActivateCamera(true);
                mPlayerCameraGO->SetEnabled(false);
            }

            mCinematicCamera = (CameraComponent*)mCinematicCameraGO->GetComponent(ComponentType::CAMERA);
            mActiveCinematicCamera = true;

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

bool CinematicCamera::HandleFadeOut(GameObject* dummy)
{
    if (mFadeOn)
    {
        if (Fade(true))
        {
            EndCinematic(dummy);
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

    return false;
}

void CinematicCamera::HandleCameraMovement()
{
    float deltaTime = App->GetDt();

    if (!mMoveCompleted)
    {
        float3 currentPosition = mCinematicCameraGO->GetWorldPosition();
        float3 direction = mTargetPosition - currentPosition;
        float distance = direction.Length();
        direction.Normalize();

        float step = mSpeedFactor * deltaTime;

        if (step >= distance)
        {
            mCinematicCameraGO->SetWorldPosition(mTargetPosition);
            mMoveCompleted = true;
        }
        else
        {
            float3 newPosition = Lerp(currentPosition, mTargetPosition, mSpeedFactor * deltaTime);

            if (newPosition.y < currentPosition.y)
            {
                newPosition.y = currentPosition.y;
            }

            mCinematicCameraGO->SetWorldPosition(newPosition);
        }
    }
}

void CinematicCamera::HandleEscape(GameObject* dummy)
{
    if (!mEscape)
    {
        if ((App->GetInput()->GetKey(Keys::Keys_B) == KeyState::KEY_DOWN) ||
            (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_DOWN))
        {
            mEscape = true;
            mFadeStart = false;
            mTravelling = false;

            mTimer.Reset();
            EndCinematic(dummy);
        }
    }
}

void CinematicCamera::EndCinematic(GameObject* dummy)
{
    if (mActiveCinematicCamera)
    {
        mActiveCinematicCamera = false;
        App->GetCamera()->RemoveEnabledCamera(mCinematicCamera);
        ActivateCamera(false);
        mPlayerCameraGO->SetEnabled(true);
    }
    
    if (dummy->GetName() == "FinalBoss")
    {
        mAnimationComponent->SetIsPlaying(true);
    }
    else
    {
      ActivateDummy(dummy, false);
    }

    if (App->GetScene()->GetName() == "Level1Scene")
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

void CinematicCamera::InitAnimation(int animState)
{
    std::string trigger;

    switch (animState)
    {
        case 1: trigger = "tIdle"; break;
        case 2: trigger = "tChase"; break;
        case 3: trigger = "tCharge"; break;
        case 4: trigger = "tAttack"; break;
        case 5: trigger = "tWakeUp"; break;

        default: trigger = "tIdle"; break;
    }

    if (mAnimationComponent)
    {
        mAnimationComponent->ResetAnimationComponent();
        mAnimationComponent->SendTrigger(trigger, 0.0f); //0.0f, 0.2, 1.0f
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

    return false;
}

void CinematicCamera::ActivateCamera(bool state)
{
    if (mCinematicCameraGO)
    {
        mCinematicCameraGO->SetEnabled(state);
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



