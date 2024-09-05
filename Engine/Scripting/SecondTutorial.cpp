#include "SecondTutorial.h"

#include "GameObject.h"
#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleInput.h"
#include "GameManager.h"
#include "Keys.h"

CREATE(SecondTutorial)
{
    CLASS(owner);
    SEPARATOR("CONTROLLER");
    MEMBER(MemberType::GAMEOBJECT, mSecondaryTutorialCon);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeTutorialCon);
    MEMBER(MemberType::GAMEOBJECT, mUltimateTutorialCon);
    SEPARATOR("KEYBOARD");
    MEMBER(MemberType::GAMEOBJECT, mSecondaryTutorial);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeTutorial);
    MEMBER(MemberType::GAMEOBJECT, mUltimateTutorial);
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
    if (GameManager::GetInstance()->UsingController())
    {
        if (mSecondaryTutorialCon && mGrenadeTutorialCon && mUltimateTutorialCon)
        {
            mSecondaryTutorialCon->SetEnabled(true);
            mCurrentStep = 1;
        }
    }
    else
    {
        if (mSecondaryTutorial && mGrenadeTutorial && mUltimateTutorial)
        {
            mSecondaryTutorial->SetEnabled(true);
            mCurrentStep = 1;
        }
    }
}

void SecondTutorial::Update()
{
    if (!mCompleted)
    {
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
        case 1:
            if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN)
            {
                //mMoveTutorialCon->SetEnabled(false);
                mSecondaryTutorial->SetEnabled(false);
                mGrenadeTutorial->SetEnabled(true);
                mCurrentStep = 2;
            }
            break;
        case 2:
            if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN)
            {
                //mShootTutorialCon->SetEnabled(false);
                mGrenadeTutorial->SetEnabled(false);
                mUltimateTutorial->SetEnabled(true);
                mCurrentStep = 3;
            }
            break;
        case 3:
            if (App->GetInput()->GetKey(Keys::Keys_C) == KeyState::KEY_DOWN)
            {
                //mDashTutorialCon->SetEnabled(false);
                mUltimateTutorial->SetEnabled(false);
                mCompleted = true;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        switch (mCurrentStep)
        {
        case 1:
            if (App->GetInput()->GetGameControllerTrigger(LEFT_TRIGGER) == ButtonState::BUTTON_DOWN)
            {
                //mMoveTutorial->SetEnabled(false);
                mSecondaryTutorialCon->SetEnabled(false);
                mGrenadeTutorialCon->SetEnabled(true);
                mCurrentStep = 2;
            }
            break;
        case 2:
            if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_DOWN)
            {
                //mShootTutorial->SetEnabled(false);
                mGrenadeTutorialCon->SetEnabled(false);
                mUltimateTutorialCon->SetEnabled(true);
                mCurrentStep = 3;
            }
            break;
        case 3:
            if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == ButtonState::BUTTON_DOWN)
            {
                //mDashTutorialCon->SetEnabled(false);
                mUltimateTutorialCon->SetEnabled(false);
                mCompleted = true;
            }
            break;
        default:
            break;
        }
    }
}
