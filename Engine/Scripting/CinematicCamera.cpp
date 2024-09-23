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
    MEMBER(MemberType::GAMEOBJECT, mEnemyGO1);
    MEMBER(MemberType::INT, mEnemy1AnimState);
    MEMBER(MemberType::GAMEOBJECT, mEnemyGO2);
    MEMBER(MemberType::INT, mEnemy2AnimState);
    MEMBER(MemberType::GAMEOBJECT, mEnemyGO3);
    MEMBER(MemberType::INT, mEnemy3AnimState);
    MEMBER(MemberType::GAMEOBJECT, mEnemyGO4);
    MEMBER(MemberType::INT, mEnemy4AnimState);
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
    }

    if (mBattleAreaGO2)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO2->GetComponent(ComponentType::SCRIPT);
        mBattleArea2 = (BattleArea*)script->GetScriptInstance();
    }

    if (mBattleAreaGO3)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO3->GetComponent(ComponentType::SCRIPT);
        mBattleArea3 = (BattleArea*)script->GetScriptInstance();
    }

    if (mBattleAreaGO4)
    {
        ScriptComponent* script = (ScriptComponent*)mBattleAreaGO4->GetComponent(ComponentType::SCRIPT);
        mBattleArea4 = (BattleArea*)script->GetScriptInstance();
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
                StartCinematic(mCinematicCameraGO1, mEnemyGO1, mEnemy1AnimState);
            }    
        }
    }

    if (mBattleArea2)
    {
        if (mBattleArea2->GetIsAreaActive())
        {
            if (mCinematicIndex == 2)
            {
                StartCinematic(mCinematicCameraGO2, mEnemyGO2, mEnemy2AnimState);
            }            
        }
    }
    
    if (mBattleArea3)
    {
        if (mBattleArea3->GetIsAreaActive())
        {
            if (mCinematicIndex == 3)
            {
                StartCinematic(mCinematicCameraGO3, mEnemyGO3, mEnemy3AnimState);
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
                StartCinematic(mCinematicCameraGO4, mEnemyGO4, mEnemy4AnimState);
            }
        }
    }
}

void CinematicCamera::StartCinematic(GameObject* camera, GameObject* target, int animState)
{
    if ((target) && (camera))
    {
        if (!mStartParameters)
        {
            mStartParameters = true;
            
            mTargetPosition = ((target->GetWorldPosition()) - ((camera->GetFront()) * mDistanceToEnemy));
            camera->Translate(-(camera->GetFront()) * mDistanceToEnemy);

            mAnimationComponent = static_cast<AnimationComponent*>(target->GetComponent(ComponentType::ANIMATION));

            if (mAnimationComponent)
            {
                mAnimationComponent->SetIsPlaying(true);
            }

            InitAnimation(animState);

            //GameManager::GetInstance()->SetPaused(true, false);
                    
            mFadeOn = true;
            mHudGO->SetEnabled(false);
            ActivateBattleArea(false);
        }

        UpdateCinematic(camera);
    }
}

void CinematicCamera::UpdateCinematic(GameObject* camera)
{
    if (mPlayingCinematic)
    {
        if (mTravelling)
        {   
            if (HandleFadeIn(camera))
            {
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
            if (HandleFadeOut(camera))
            {
                mTravelling = true;
                mPlayingCinematic = false;
            }
        }

        HandleEscape();     
    }
    else
    {
        mCinematicIndex++;
        mStartParameters = false;
        mPlayingCinematic = true;
        mHudGO->SetEnabled(true);
        ActivateBattleArea(true);
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
                ActivateCamera(camera);
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

bool CinematicCamera::HandleFadeOut(GameObject* camera)
{
    if (mFadeOn)
    {
        if (Fade(true))
        {
            EndCinematic();
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

void CinematicCamera::HandleEscape()
{
    if (!mEscape)
    {
        if ((App->GetInput()->GetKey(Keys::Keys_B) == KeyState::KEY_REPEAT) ||
            (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_REPEAT))
        {
            mEscape = true;
            mFadeStart = false;
            mTimer.Reset();
            EndCinematic();
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

void CinematicCamera::EndCinematic()
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

    if (mBattleArea1)
    {
        if (mBattleArea1->GetIsAreaActive())
        {
            mEnemyGO1->SetEnabled(false);
        }
    }

    if (mBattleArea2)
    {
        if (mBattleArea2->GetIsAreaActive())
        {
            mEnemyGO2->SetEnabled(false);
        }
    }

    if (mBattleArea3)
    {
        if (mBattleArea3->GetIsAreaActive())
        {
            mEnemyGO3->SetEnabled(false);
        }
    }

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
    if (mCinematicCameraGO1)
    {
        mCinematicCameraGO1->SetEnabled(false);
    }

    if (mCinematicCameraGO2)
    {
        mCinematicCameraGO2->SetEnabled(false);
    }

    if (mCinematicCameraGO3)
    {
        mCinematicCameraGO3->SetEnabled(false);
    }

    if (mCinematicCameraGO4)
    {
        mCinematicCameraGO4->SetEnabled(false);
    }
}

void CinematicCamera::ActivateCamera(GameObject* cinematicCamera)
{
    if (cinematicCamera)
    {
        cinematicCamera->SetEnabled(true);
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

void CinematicCamera::ActivateBattleArea(bool state)
{
    if (mBattleArea1)
    {
        mBattleArea1->SetSpawnEnemies(state);
    }
    
    if (mBattleArea2)
    {
        mBattleArea2->SetSpawnEnemies(state);
    }

    if (mBattleArea3)
    {
        mBattleArea3->SetSpawnEnemies(state);
    }

    if (mBattleArea4)
    {
        mBattleArea4->SetSpawnEnemies(state);
    }
}