#include "FirstTutorial.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleInput.h"
#include "GameManager.h"
#include "Keys.h"

CREATE(FirstTutorial)
{
    CLASS(owner);
    SEPARATOR("CONTROLLER");
    MEMBER(MemberType::GAMEOBJECT, mMoveTutorialCon);
    MEMBER(MemberType::GAMEOBJECT, mShootTutorialCon);
    MEMBER(MemberType::GAMEOBJECT, mDashTutorialCon);
    SEPARATOR("KEYBOARD");
    MEMBER(MemberType::GAMEOBJECT, mMoveTutorial);
    MEMBER(MemberType::GAMEOBJECT, mShootTutorial);
    MEMBER(MemberType::GAMEOBJECT, mDashTutorial);
    END_CREATE;
}

FirstTutorial::FirstTutorial(GameObject* owner) : Script(owner)
{
}

FirstTutorial::~FirstTutorial()
{
}

void FirstTutorial::Start()
{
    if (GameManager::GetInstance()->UsingController()) 
    {
        if (mMoveTutorialCon && mShootTutorialCon && mDashTutorialCon)
        {
            mMoveTutorialCon->SetEnabled(true);
            mCurrentStep = 1;
        }
    }
    else
    {
        if (mMoveTutorial && mShootTutorial && mDashTutorial)
        {
            mMoveTutorial->SetEnabled(true);
            mCurrentStep = 1;
        }    
    }
}

void FirstTutorial::Update()
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

void FirstTutorial::Tutorial()
{
    if (!GameManager::GetInstance()->UsingController())
    {
        switch (mCurrentStep)
        {
        case 1:
            if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
                App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
                App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
                App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
            {
                //mMoveTutorialCon->SetEnabled(false);
                mMoveTutorial->SetEnabled(false);
                mShootTutorial->SetEnabled(true);
                mCurrentStep = 2;
            }
            break;
        case 2:
            if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
            {
                //mShootTutorialCon->SetEnabled(false);
                mShootTutorial->SetEnabled(false);
                mDashTutorial->SetEnabled(true);
                mCurrentStep = 3;
            }
            break;
        case 3:
            if (App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_DOWN)
            {
                //mDashTutorialCon->SetEnabled(false);
                mDashTutorial->SetEnabled(false);
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
            if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != 0 ||
                App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != 0)
            {
                //mMoveTutorial->SetEnabled(false);
                mMoveTutorialCon->SetEnabled(false);
                mShootTutorialCon->SetEnabled(true);
                mCurrentStep = 2;
            }
            break;
        case 2:
            if (App->GetInput()->GetGameControllerTrigger(RIGHT_TRIGGER) == ButtonState::BUTTON_DOWN)
            {
                //mShootTutorial->SetEnabled(false);
                mShootTutorialCon->SetEnabled(false);
                mDashTutorial->SetEnabled(true);
                mCurrentStep = 3;
            }
            break;
        case 3:
            if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
            {
                //mDashTutorialCon->SetEnabled(false);
                mDashTutorial->SetEnabled(false);
                mCompleted = true;
            }
            break;
        default:
            break;
        }
    }
}
