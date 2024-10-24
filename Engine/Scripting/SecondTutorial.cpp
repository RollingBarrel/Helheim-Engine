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
    MEMBER(MemberType::GAMEOBJECT, mCollectibleTutCon);
    MEMBER(MemberType::GAMEOBJECT, mSkipTutorialCon);
    MEMBER(MemberType::GAMEOBJECT, mContinueCon);
    SEPARATOR("KEYBOARD");
    MEMBER(MemberType::GAMEOBJECT, mShootTutorial);
    MEMBER(MemberType::GAMEOBJECT, mSecondaryTutorial);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeTutorial);
    MEMBER(MemberType::GAMEOBJECT, mUltimateTutorial);
    MEMBER(MemberType::GAMEOBJECT, mCollectibleTut);
    MEMBER(MemberType::GAMEOBJECT, mSkipTutorial);
    MEMBER(MemberType::GAMEOBJECT, mContinue);
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
    if (mTutorialAreaGO) {
        mTutorialArea = (BattleArea*)((ScriptComponent*)mTutorialAreaGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance(); 
        GameManager::GetInstance()->SetActiveBattleArea(mTutorialArea);
    }
        
  

    if (GameManager::GetInstance()->UsingController())
    {
        if (mSecondaryTutorialCon && mGrenadeTutorialCon && mUltimateTutorialCon && mShootTutorialCon)
        {
            GameManager::GetInstance()->SetPaused(true, false, false);
            mContinueCon->SetEnabled(true);
            mShootTutorialCon->SetEnabled(true);
        }
    }
    else
    {
        if (mSecondaryTutorial && mGrenadeTutorial && mUltimateTutorial && mShootTutorial)
        {
            GameManager::GetInstance()->SetPaused(true, false, false);
            mContinue->SetEnabled(true);
            mShootTutorial->SetEnabled(true);
        }
    }

}

void SecondTutorial::Update()
{
    if (!mPart1Completed)
    {
        if (mTutorialArea) mCurrentStep = mTutorialArea->GetCurrentWave();
        CheckKill();
        Tutorial();

        if (mTutorialTimer.DelayWithoutReset(1.0f) && 
            (App->GetInput()->GetKey(Keys::Keys_X) == KeyState::KEY_DOWN || App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_DOWN))
        {
            mPart1Completed = true;
            DisableFirstPart();
            mTutorialArea->SetWaves(1);
            GameManager::GetInstance()->UnlockSecondary();
            GameManager::GetInstance()->UnlockGrenade(true);
            GameManager::GetInstance()->UnlockUltimate(true);
            GameManager::GetInstance()->SetPaused(false, false, false);
        }
    }
    else
    {
        DisableFirstPart();
        if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN||
            App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
        {
            GameManager::GetInstance()->SetPaused(false, false, false);
            mCollectibleTutCon->SetEnabled(false);
            mCollectibleTut->SetEnabled(false);
            mContinue->SetEnabled(false);
            mContinueCon->SetEnabled(false);
            GameManager::GetInstance()->UnlockUltimate(true);
        }
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
        if (mSkipTutorial)
        {
            mSkipTutorial->SetEnabled(true);
            mSkipTutorialCon->SetEnabled(false);
        }
        switch (mCurrentStep)
        {
        case 4:
            if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN)
            {
                GameManager::GetInstance()->SetPaused(false, false, false);
                mShootTutorialCon->SetEnabled(false);
                mShootTutorial->SetEnabled(false);
                mContinue->SetEnabled(false);
            }
            break;
        case 3:
            if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN)
            {
                mSecondaryTutorialCon->SetEnabled(false);
                mSecondaryTutorial->SetEnabled(false);
                mContinue->SetEnabled(false);
                GameManager::GetInstance()->SetPaused(false, false, false);
            }
            break;
        case 2:
            if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN)
            {
                mGrenadeTutorialCon->SetEnabled(false);
                mGrenadeTutorial->SetEnabled(false);
                mContinue->SetEnabled(false);
                mGrenadeUsed = true;
                GameManager::GetInstance()->SetPaused(false, false, false);
            }
            break;
        case 1:
            if (mGrenadeUsed)
            {
                mCollectibleTut->SetEnabled(true);
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
        if (mSkipTutorialCon)
        {
            mSkipTutorial->SetEnabled(false);
            mSkipTutorialCon->SetEnabled(true);
        }
        switch (mCurrentStep)
        {
        case 4:
            if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
            {
                GameManager::GetInstance()->SetPaused(false, false, false);
                mShootTutorialCon->SetEnabled(false);
                mShootTutorial->SetEnabled(false);
                mContinueCon->SetEnabled(false);
            }
            break;
        case 3:
            if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
            {
                mSecondaryTutorialCon->SetEnabled(false);
                mSecondaryTutorial->SetEnabled(false);
                mContinueCon->SetEnabled(false);
                GameManager::GetInstance()->SetPaused(false, false, false);
            }
            break;
        case 2:
            if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
            {
                mGrenadeTutorialCon->SetEnabled(false);
                mGrenadeTutorial->SetEnabled(false);
                mContinueCon->SetEnabled(false);
                mGrenadeUsed = true;
                GameManager::GetInstance()->SetPaused(false, false, false);
            }
            break;
        case 1:
            if (mGrenadeUsed)
            {
                mCollectibleTutCon->SetEnabled(true);
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

void SecondTutorial::CheckKill()
{
    if (mKills > mCurrentStep)
    {
        mKills--;
        GameManager::GetInstance()->SetPaused(true, false, false);
        if (!GameManager::GetInstance()->UsingController())
        {
            mContinue->SetEnabled(true);
            switch (mCurrentStep)
            {
            case 3:
                mSecondaryTutorialCon->SetEnabled(false);
                mSecondaryTutorial->SetEnabled(true);
                GameManager::GetInstance()->UnlockSecondary();
                break;
            case 2:
                mGrenadeTutorialCon->SetEnabled(false);
                mGrenadeTutorial->SetEnabled(true);
                GameManager::GetInstance()->UnlockGrenade(true);
                break;
            default:
                break;
            }
        }
        else
        {
            mContinueCon->SetEnabled(true);
            switch (mCurrentStep)
            {
            case 3:
                mSecondaryTutorialCon->SetEnabled(true);
                mSecondaryTutorial->SetEnabled(false);
                GameManager::GetInstance()->UnlockSecondary();
                break;
            case 2:
                mGrenadeTutorialCon->SetEnabled(true);
                mGrenadeTutorial->SetEnabled(false);
                GameManager::GetInstance()->UnlockGrenade(true);
                break;
            default:
                break;
            }
        }
        
    }
}

void SecondTutorial::UltTutorial()
{
    int ultResource = GameManager::GetInstance()->GetPlayerController()->GetUltimateResource();
    if (GameManager::GetInstance()->UsingController())
    {
        if (ultResource >= 100 && !mUltTutorialStarted)
        {
            GameManager::GetInstance()->SetPaused(true, false, false);
            if (mSkipTutorialCon)
            {
                mSkipTutorial->SetEnabled(false);
                mSkipTutorialCon->SetEnabled(true);
            }
            mUltimateTutorial->SetEnabled(false);
            mUltimateTutorialCon->SetEnabled(true);
            mContinueCon->SetEnabled(true);
            mUltTutorialStarted = true;
        }
        else
        {
            if (mUltTutorialStarted && App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
            {
                GameManager::GetInstance()->SetPaused(false, false, false);
                mContinueCon->SetEnabled(false);
                mPart2Completed = true;
            }
        }
    }
    else
    {
        
        if (ultResource >= 100 && !mUltTutorialStarted)
        {
            GameManager::GetInstance()->SetPaused(true, false, false);
            if (mSkipTutorial)
            {
                mSkipTutorial->SetEnabled(true);
                mSkipTutorialCon->SetEnabled(false);
            }
            GameManager::GetInstance()->SetPaused(true, false, false);
            mUltimateTutorialCon->SetEnabled(false);
            mUltimateTutorial->SetEnabled(true);
            mContinue->SetEnabled(true);
            mUltTutorialStarted = true;
        }
        else
        {
            if (mUltTutorialStarted && App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN)
            {
                GameManager::GetInstance()->SetPaused(false, false, false);
                mContinue->SetEnabled(false);
                mPart2Completed = true;
            }
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
    mSkipTutorial->SetEnabled(false);
    mSkipTutorialCon->SetEnabled(false);
}
