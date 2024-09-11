#include "SecondTutorial.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleInput.h"
#include "GameManager.h"
#include "Keys.h"
#include "BattleArea.h"

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
    if (!mCompleted)
    {
        if (mTutorialArea) mCurrentStep = mTutorialArea->GetCurrentWave();
        Tutorial(); 
        if (App->GetInput()->GetKey(Keys::Keys_X) == KeyState::KEY_DOWN || App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_X) == ButtonState::BUTTON_DOWN)
        {
            mCompleted = true;
            //mTutorialArea->SetSpawnerCycles(3);
            mTutorialArea->SetWaves(3);
            GameManager::GetInstance()->UnlockSecondary();
            GameManager::GetInstance()->UnlockGrenade(true);
            GameManager::GetInstance()->UnlockUltimate(true);
        }
    }
    else
    {
        mGameObject->SetEnabled(false);
    }
}

void SecondTutorial::Tutorial()
{
    if (!GameManager::GetInstance()->UsingController())
    {
        switch (mCurrentStep)
        {
        case 5:
            mShootTutorial->SetEnabled(false);
            mSecondaryTutorial->SetEnabled(true);
            GameManager::GetInstance()->UnlockSecondary();
            if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN)
                mSecondaryUsed = true;
            break;
        case 4:
            if (mSecondaryUsed)
            {
                mSecondaryTutorial->SetEnabled(false);
                mGrenadeTutorial->SetEnabled(true);
                GameManager::GetInstance()->UnlockGrenade(true);
                if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN)
                    mGrenadeUsed = true;
            }
            else
                mTutorialArea->SetWaves(mTutorialArea->GetCurrentWave() + 1);
            break;
        case 3:
            if (mGrenadeUsed)
            {
                mGrenadeTutorial->SetEnabled(false);
                mUltimateTutorial->SetEnabled(true);
                GameManager::GetInstance()->UnlockUltimate(true);
                if (App->GetInput()->GetKey(Keys::Keys_C) == KeyState::KEY_DOWN)
                    mUltimateUsed = true;
            }
            else
                mTutorialArea->SetWaves(mTutorialArea->GetCurrentWave() + 1);
            break;
        case 2:
            if (mUltimateUsed)
            {
                mUltimateTutorial->SetEnabled(false);
                mCompleted = true;
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
        if (mSkipTutorialCon)
        {
            mSkipTutorialCon->SetEnabled(true);
            if (mSkipTutorial) mSkipTutorial->SetEnabled(false);
        }
        switch (mCurrentStep)
        {
        case 5:
            mShootTutorialCon->SetEnabled(false);
            mSecondaryTutorialCon->SetEnabled(true);
            GameManager::GetInstance()->UnlockSecondary();
            if (App->GetInput()->GetGameControllerTrigger(LEFT_TRIGGER) == ButtonState::BUTTON_DOWN)
                mSecondaryUsed = true;
            break;
        case 4:
            if (mSecondaryUsed)
            {
                mSecondaryTutorialCon->SetEnabled(false);
                mGrenadeTutorialCon->SetEnabled(true);
                GameManager::GetInstance()->UnlockGrenade(true);
                if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_DOWN)
                    mGrenadeUsed = true;
            }
            else
                mTutorialArea->SetWaves(mTutorialArea->GetCurrentWave() + 1);
            break;
        case 3:
            if (mGrenadeUsed)
            {
                mGrenadeTutorialCon->SetEnabled(false);
                mUltimateTutorialCon->SetEnabled(true);
                GameManager::GetInstance()->UnlockUltimate(true);
                if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == ButtonState::BUTTON_DOWN)
                    mUltimateUsed = true;
            }
            else
                mTutorialArea->SetWaves(mTutorialArea->GetCurrentWave() + 1);
            break;
        case 2:
            if (mUltimateUsed)
            {
                mUltimateTutorialCon->SetEnabled(false);
                mCompleted = true;
            }
            else
                mTutorialArea->SetWaves(mTutorialArea->GetCurrentWave() + 1);
            break;
        default:
            break;
        }
    }
}
