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
            mCurrentStep = 2;
            break;
        case 4:
              //mMoveTutorialCon->SetEnabled(false);
            mSecondaryTutorial->SetEnabled(false);
            mGrenadeTutorial->SetEnabled(true);
            mCurrentStep = 2;
            GameManager::GetInstance()->UnlockGrenade(true);
            break;
        case 3:
            //mShootTutorialCon->SetEnabled(false);
            mGrenadeTutorial->SetEnabled(false);
            mUltimateTutorial->SetEnabled(true);
            mCurrentStep = 3;
            GameManager::GetInstance()->UnlockUltimate(true);
            
            break;
        case 2:
            //mDashTutorialCon->SetEnabled(false);
            mUltimateTutorial->SetEnabled(false);
            mCompleted = true;
            break;
        default:
            break;
        }
    }
    else
    {
        switch (mCurrentStep)
        {
        case 5:
            mShootTutorialCon->SetEnabled(false);
            mSecondaryTutorialCon->SetEnabled(true);
            GameManager::GetInstance()->UnlockSecondary();
            mCurrentStep = 2;
            break;
        case 4:
            //mMoveTutorialCon->SetEnabled(false);
            mSecondaryTutorialCon->SetEnabled(false);
            mGrenadeTutorialCon->SetEnabled(true);
            mCurrentStep = 2;
            GameManager::GetInstance()->UnlockGrenade(true);
            break;
        case 3:
            //mShootTutorialCon->SetEnabled(false);
            mGrenadeTutorialCon->SetEnabled(false);
            mUltimateTutorialCon->SetEnabled(true);
            mCurrentStep = 3;
            GameManager::GetInstance()->UnlockUltimate(true);

            break;
        case 2:
            //mDashTutorialCon->SetEnabled(false);
            mUltimateTutorialCon->SetEnabled(false);
            mCompleted = true;
            break;
        default:
            break;
        }
    }
}
