#include "GameManager.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "ButtonComponent.h"
#include "ImageComponent.h"
#include "MainMenu.h"

CREATE(GameManager)
{
    CLASS(owner);
    SEPARATOR("Screens");
    MEMBER(MemberType::GAMEOBJECT, mPauseScreen);
    MEMBER(MemberType::GAMEOBJECT, mWinScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoseScreen);
    SEPARATOR("Buttons");
    MEMBER(MemberType::GAMEOBJECT, mYesGO);
    MEMBER(MemberType::GAMEOBJECT, mNoGO);
    END_CREATE;
}

GameManager::GameManager(GameObject* owner) : Script(owner) {}

void GameManager::Start() 
{
    mPauseScreen->SetEnabled(false);
    mWinScreen->SetEnabled(false);
    mLoseScreen->SetEnabled(false);

    mWinBtn = static_cast<ButtonComponent*>(mWinScreen->GetComponent(ComponentType::BUTTON));
    mLoseBtn = static_cast<ButtonComponent*>(mLoseScreen->GetComponent(ComponentType::BUTTON));
    mYesBtn = static_cast<ButtonComponent*>(mYesGO->GetComponent(ComponentType::BUTTON));
    mNoBtn = static_cast<ButtonComponent*>(mNoGO->GetComponent(ComponentType::BUTTON));

    mWinBtn->AddEventHandler(EventType::CLICK, std::bind(&GameManager::OnWinButtonClick, this));
    mLoseBtn->AddEventHandler(EventType::CLICK, std::bind(&GameManager::OnLoseButtonClick, this));
    mYesBtn->AddEventHandler(EventType::CLICK, std::bind(&GameManager::OnYesButtonClick, this));
    mNoBtn->AddEventHandler(EventType::CLICK, std::bind(&GameManager::OnNoButtonClick, this));

    mYesBtn->AddEventHandler(EventType::HOVER, std::bind(&GameManager::OnYesButtonHoverOn, this));
    mNoBtn->AddEventHandler(EventType::HOVER, std::bind(&GameManager::OnNoButtonHoverOn, this));
    mYesBtn->AddEventHandler(EventType::HOVEROFF, std::bind(&GameManager::OnYesButtonHoverOff, this));
    mNoBtn->AddEventHandler(EventType::HOVEROFF, std::bind(&GameManager::OnNoButtonHoverOff, this));
}

void GameManager::Update()
{
    Controls();
}

bool GameManager::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}

void GameManager::Controls()
{
    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
    {  
        mPaused = !mPaused;
        mPauseScreen->SetEnabled(mPaused);
    }
}

void GameManager::WinScreen() 
{
    mWinScreen->SetEnabled(true);
}

void GameManager::LoseScreen() 
{
    mLoseScreen->SetEnabled(true);
}

void GameManager::OnWinButtonClick() 
{
    App->GetScene()->Load("MainMenu");
}

void GameManager::OnLoseButtonClick() 
{
    App->GetScene()->Load("MainMenu");
}

void GameManager::OnYesButtonClick() 
{
    App->GetScene()->Load("MainMenu");
}

void GameManager::OnNoButtonClick() 
{
    mPaused = false;
    mPauseScreen->SetEnabled(false);
}

void GameManager::OnYesButtonHoverOn()
{
    ImageComponent* image = static_cast<ImageComponent*>(mYesGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(0.7f, 0.7f, 0.7f));
}

void GameManager::OnNoButtonHoverOn()
{
    ImageComponent* image = static_cast<ImageComponent*>(mNoGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(0.7f, 0.7f, 0.7f));
}

void GameManager::OnYesButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mYesGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(1, 1, 1));
}

void GameManager::OnNoButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mNoGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(1, 1, 1));
}
