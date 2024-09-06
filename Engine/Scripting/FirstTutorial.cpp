#include "FirstTutorial.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "Application.h"
#include "ModuleInput.h"
#include "GameManager.h"
#include "AnimationComponent.h"
#include "BoxColliderComponent.h"
#include "Keys.h"

CREATE(FirstTutorial)
{
    CLASS(owner);
    SEPARATOR("CONTROLLER");
    MEMBER(MemberType::GAMEOBJECT, mMoveTutorialCon);
    MEMBER(MemberType::GAMEOBJECT, mDashTutorialCon);
    SEPARATOR("KEYBOARD");
    MEMBER(MemberType::GAMEOBJECT, mMoveTutorial);
    MEMBER(MemberType::GAMEOBJECT, mDashTutorial);
    SEPARATOR("DOOR");
    MEMBER(MemberType::GAMEOBJECT, mDoor1);
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
        if (mMoveTutorialCon && mDashTutorialCon)
        {
            mMoveTutorialCon->SetEnabled(true);
            mCurrentStep = 1;
        }
    }
    else
    {
        if (mMoveTutorial && mDashTutorial)
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
        if (mDoor1)
        {
            AnimationComponent* doorAnimation1 = static_cast<AnimationComponent*>(mDoor1->GetComponent(ComponentType::ANIMATION));
            if (doorAnimation1)
            {
                doorAnimation1->SetIsPlaying(true);
                doorAnimation1->SendTrigger("tClose", 0.6f);

            }

            BoxColliderComponent* door1Collider = static_cast<BoxColliderComponent*>(mDoor1->GetComponent(ComponentType::BOXCOLLIDER));
            if (door1Collider)
            {
                door1Collider->SetEnable(true);
            }
        }
    }
    else 
    {
        if (mDoor1)
        {
            AnimationComponent* doorAnimation1 = static_cast<AnimationComponent*>(mDoor1->GetComponent(ComponentType::ANIMATION));
            if (doorAnimation1)
            {
                doorAnimation1->SetIsPlaying(true);
                doorAnimation1->SendTrigger("tOpen", 0.6f);

            }

            BoxColliderComponent* door1Collider = static_cast<BoxColliderComponent*>(mDoor1->GetComponent(ComponentType::BOXCOLLIDER));
            if (door1Collider)
            {
                door1Collider->SetEnable(false);
            }
        }
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
                mDashTutorial->SetEnabled(true);
                mCurrentStep = 2;
            }
            break;
        case 2:
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
                mDashTutorialCon->SetEnabled(true);
                mCurrentStep = 2;
            }
            break;
        case 2:
            if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
            {
                //mDashTutorialCon->SetEnabled(false);
                mDashTutorialCon->SetEnabled(false);
                mCompleted = true;
            }
            break;
        default:
            break;
        }
    }
}
