#include "SecondTutorial.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleInput.h"
#include "GameManager.h"
#include "Keys.h"
#include "BattleArea.h"
#include "PlayerController.h"

CREATE(SecondTutorial)
{
    CLASS(owner);
    SEPARATOR("CONTROLLER");
    MEMBER(MemberType::GAMEOBJECT, mShootTutorialCon);
    MEMBER(MemberType::GAMEOBJECT, mSecondaryTutorialCon);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeTutorialCon);
    MEMBER(MemberType::GAMEOBJECT, mUltimateTutorialCon);
    SEPARATOR("KEYBOARD");
    MEMBER(MemberType::GAMEOBJECT, mShootTutorial);
    MEMBER(MemberType::GAMEOBJECT, mSecondaryTutorial);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeTutorial);
    MEMBER(MemberType::GAMEOBJECT, mUltimateTutorial);
    SEPARATOR("AREA");
    MEMBER(MemberType::GAMEOBJECT, mTutorialAreaGO);
    END_CREATE;
}

SecondTutorial::SecondTutorial(GameObject* owner) : Script(owner)
{
}

SecondTutorial::~SecondTutorial()
{
}

void SecondTutorial::Start()
{
    if (mTutorialAreaGO)
        mTutorialArea = (BattleArea*)((ScriptComponent*)mTutorialAreaGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
  
    if (GameManager::GetInstance()->UsingController())
    {
        if (mSecondaryTutorialCon && mGrenadeTutorialCon && mUltimateTutorialCon && mShootTutorialCon)
        {
            mShootTutorialCon->SetEnabled(true);
        }
    }
    else
    {
        if (mSecondaryTutorial && mGrenadeTutorial && mUltimateTutorial && mShootTutorial)
        {
            mShootTutorial->SetEnabled(true);
        }
    }
}

void SecondTutorial::Update()
{
    if (!mPart1Completed)
    {
        if (mTutorialArea) mCurrentStep = mTutorialArea->GetCurrentWave();
        Tutorial(); 
        if (App->GetInput()->GetKey(Keys::Keys_X) == KeyState::KEY_DOWN || App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_X) == ButtonState::BUTTON_DOWN)
        {
            mPart1Completed = true;
            DisableFirstPart();
            mTutorialArea->SetWaves(1);
            GameManager::GetInstance()->UnlockSecondary();
            GameManager::GetInstance()->UnlockGrenade(true);
        }
    }
    else
    {
        DisableFirstPart();
        GameManager::GetInstance()->UnlockUltimate(true);
        if (!mPart2Completed)
        {
            UltTutorial();
        }
        else
        {
            mGameObject->SetEnabled(false);
        }
    }
}

void SecondTutorial::Tutorial()
{
    if (!GameManager::GetInstance()->UsingController())
    {
        switch (mCurrentStep)
        {
        case 3:
            mShootTutorialCon->SetEnabled(false);
            mShootTutorial->SetEnabled(false);
            mSecondaryTutorial->SetEnabled(true);
            GameManager::GetInstance()->UnlockSecondary();
            break;
        case 2:
            mSecondaryTutorialCon->SetEnabled(false);
            mSecondaryTutorial->SetEnabled(false);
            mGrenadeTutorialCon->SetEnabled(false);
            mGrenadeTutorial->SetEnabled(true);
            GameManager::GetInstance()->UnlockGrenade(true);
            if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN)
                mGrenadeUsed = true;
            break;
        case 1:
            if (mGrenadeUsed)
            {
                mGrenadeTutorialCon->SetEnabled(false);
                mGrenadeTutorial->SetEnabled(false);
                mPart1Completed = true;
            }
            else
                mTutorialArea->SetWaves(mTutorialArea->GetCurrentWave() + 1);
            break;
        default:
            break;
        }
    }
    else
    {
        switch (mCurrentStep)
        {
        case 3:
            mShootTutorialCon->SetEnabled(false);
            mShootTutorial->SetEnabled(false);
            mSecondaryTutorial->SetEnabled(true);
            GameManager::GetInstance()->UnlockSecondary();
            break;
        case 2:
            mSecondaryTutorialCon->SetEnabled(false);
            mSecondaryTutorial->SetEnabled(false);
            mGrenadeTutorialCon->SetEnabled(true);
            mGrenadeTutorial->SetEnabled(false);
            GameManager::GetInstance()->UnlockGrenade(true);
            if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_DOWN)
                mGrenadeUsed = true;
            break;
        case 1:
            if (mGrenadeUsed)
            {
                mGrenadeTutorialCon->SetEnabled(false);
                mGrenadeTutorial->SetEnabled(false);
                mPart1Completed = true;
            }
            else
                mTutorialArea->SetWaves(mTutorialArea->GetCurrentWave() + 1);
            break;
        default:
            break;
        }
    }
}

void SecondTutorial::UltTutorial()
{
    int ultResource = GameManager::GetInstance()->GetPlayerController()->GetUltimateResource();
    if (GameManager::GetInstance()->UsingController())
    {
        if (!mUltTutorialStarted && ultResource >= 100)
        {
            mUltimateTutorial->SetEnabled(false);
            mUltimateTutorialCon->SetEnabled(true);
            mUltTutorialStarted = true;
        }
        else
        {
            if (App->GetInput()->GetGameControllerTrigger(LEFT_TRIGGER) == ButtonState::BUTTON_DOWN || 
                App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_X) == ButtonState::BUTTON_DOWN) mPart2Completed = true;
        }
    }
    else
    {
        if (!mUltTutorialStarted && ultResource >= 100)
        {
            mUltimateTutorialCon->SetEnabled(false);
            mUltimateTutorial->SetEnabled(true);
            mUltTutorialStarted = true;
        }
        else
        {
            if (App->GetInput()->GetKey(Keys::Keys_C) == KeyState::KEY_DOWN ||
                App->GetInput()->GetKey(Keys::Keys_X) == KeyState::KEY_DOWN) mPart2Completed = true;
        }
    }
    
}

void SecondTutorial::DisableFirstPart()
{
    mShootTutorialCon->SetEnabled(false);
    mSecondaryTutorialCon->SetEnabled(false);
    mGrenadeTutorialCon->SetEnabled(false);
    mShootTutorial->SetEnabled(false);
    mSecondaryTutorial->SetEnabled(false);
    mGrenadeTutorial->SetEnabled(false);
}
