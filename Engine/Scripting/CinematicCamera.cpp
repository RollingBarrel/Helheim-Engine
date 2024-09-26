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
    SEPARATOR("ENEMIES");
    SEPARATOR("Idle = 1 | Chase = 2 | Charge = 3 | Attack = 4");
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

CinematicCamera* CinematicCamera::mInstance = nullptr;

CinematicCamera* CinematicCamera::GetInstance()
{
    if (mInstance == nullptr)
    {
        LOG("CinematicCamera instance has not been initialized.");
        throw std::runtime_error("CinematicCamera instance has not been initialized.");
    }
    return mInstance;
}

CinematicCamera::CinematicCamera(GameObject* owner) : Script(owner) {}

CinematicCamera::~CinematicCamera()
{
    mInstance = nullptr;
}

void CinematicCamera::Awake() 
{
    assert(!mInstance);
    mInstance = this;
}

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
                if (App->GetScene()->GetName() == "Level1Scene")
                {
                    StartCinematic(mEnemyGO1, mBattleArea1, mEnemyAnimState1, 67.21f, 1.50f, 7.83f, 0.00f, -90.00f, 0.00f);
                }

                if (App->GetScene()->GetName() == "Level2Scene")
                {
                    StartCinematic(mEnemyGO1, mBattleArea1, mEnemyAnimState1, 0.00f, 1.50f, 0.00f, 0.00f, -90.00f, 0.00f);
                }

                if (App->GetScene()->GetName() == "Level3Scene")
                {
                    StartCinematic(mEnemyGO1, mBattleArea1, mEnemyAnimState1, 25.00f, 1.50f, 0.00f, 0.00f, -90.00f, 0.00f); //12.45

                    //mDistanceToEnemy = mDistanceToEnemy + 4.5f; //Boss enemy is bigger than other enemies and needs more distance for the camera
                }       
            }    
        }
    }

    if (mBattleArea2)
    {
        if (mBattleArea2->GetIsAreaActive())
        {
            if (mCinematicIndex == 2)
            {
                if (App->GetScene()->GetName() == "Level1Scene")
                {
                    StartCinematic(mEnemyGO2, mBattleArea2, mEnemyAnimState2, 12.77f, 1.50f, -22.54f, 0.00f, -90.00f, 0.00f);
                }        
            }            
        }
    }
    
    if (mBattleArea3)
    {
        if (mBattleArea3->GetIsAreaActive())
        {
            if (mCinematicIndex == 3)
            {
                if (App->GetScene()->GetName() == "Level1Scene")
                {
                    StartCinematic(mEnemyGO3, mBattleArea3, mEnemyAnimState3, -38.75f, 1.50f, -17.11f, 0.00f, -90.00f, 0.00f);
                }             
            }
        }
    }
    
    if (mBattleArea4)
    {
        if (mBattleArea4->GetIsAreaActive())
        {
            if (mCinematicIndex == 4)
            {
                if (App->GetScene()->GetName() == "Level1Scene")
                {
                    StartCinematic(mEnemyGO4, mBattleArea4, mEnemyAnimState4, -113.00f, 1.50f, 4.00f, 0.00f, -180.00f, 0.00f);
                }
            }
        }
    }
}

void CinematicCamera::StartCinematic(GameObject* dummy, BattleArea* battleArea, int animState,
    float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
    if ((dummy) && (mCinematicCameraGO))
    {
        if (!mStartParameters)
        {
            mStartParameters = true;

            LocateCamera(posX, posY, posZ, rotX, rotY, rotZ);
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
    if (!GameManager::GetInstance()->IsPaused())
    {
        if (mPlayingCinematic)
        {
            if (mTravelling)
            {
                if (HandleFadeIn())
                {
                    if (mEnemyGO1->GetName() != "FinalBoss")
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

            HandleEscape(dummy);
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
    }
}

void CinematicCamera::LocateCamera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
    float rotXRad = DegToRad(rotX);
    float rotYRad = DegToRad(rotY);
    float rotZRad = DegToRad(rotZ);

    mCinematicCameraGO->SetWorldPosition(float3(posX, posY, posZ));
    mCinematicCameraGO->SetWorldRotation(float3(rotXRad, rotYRad, rotZRad));
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
        if ((App->GetInput()->GetKey(Keys::Keys_X) == KeyState::KEY_REPEAT) ||
            (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_X) == ButtonState::BUTTON_REPEAT))
        {
            mEscape = true;
            mFadeStart = false;
            mTravelling = false;

            mTimer.Reset();
            EndCinematic(dummy);
        }
    }

    if (mEscape)
    {
        if (Fade(false))
        {
            mEscape = false;
            mPlayingCinematic = false;
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
        App->GetCamera()->ActivateFirstCamera();
    }
    
    if (dummy->GetName()=="FinalBoss")
    {
        mAnimationComponent->SetIsPlaying(true);
    }
    else
    {
        if (mEnemyGO1->GetName() != "FinalBoss")
        {
            ActivateDummy(dummy, false);
        }
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



