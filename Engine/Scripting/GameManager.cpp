#include "pch.h"
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
    MEMBER(MemberType::GAMEOBJECT, mLoadingScreen);
    SEPARATOR("Buttons");
    MEMBER(MemberType::GAMEOBJECT, mYesGO);
    MEMBER(MemberType::GAMEOBJECT, mNoGO);
    END_CREATE;
}


GameManager* GameManager::mInstance = nullptr;

GameManager* GameManager::GetInstance()
{
    if (mInstance == nullptr) {
        LOG("GameManager instance has not been initialized.");
        throw std::runtime_error("GameManager instance has not been initialized.");
    }
    return mInstance;
}

GameManager::GameManager(GameObject* owner) : Script(owner) {}

GameManager::~GameManager()
{
    delete mInstance;
}

void GameManager::Start()
{
    mInstance = this;
    
    mInstance->mPauseScreen->SetEnabled(false);
    mInstance->mWinScreen->SetEnabled(false);
    mInstance->mLoseScreen->SetEnabled(false);
    mInstance->mLoadingScreen->SetEnabled(false);

    mInstance->mWinBtn = static_cast<ButtonComponent*>(mWinScreen->GetComponent(ComponentType::BUTTON));
    mInstance->mLoseBtn = static_cast<ButtonComponent*>(mLoseScreen->GetComponent(ComponentType::BUTTON));
    mInstance->mYesBtn = static_cast<ButtonComponent*>(mYesGO->GetComponent(ComponentType::BUTTON));
    mInstance->mNoBtn = static_cast<ButtonComponent*>(mNoGO->GetComponent(ComponentType::BUTTON));

    
    mInstance->mWinBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&GameManager::OnWinButtonClick, this)));
    mInstance->mLoseBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&GameManager::OnLoseButtonClick, this)));
    mInstance->mYesBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&GameManager::OnYesButtonClick, this)));
    mInstance->mNoBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&GameManager::OnNoButtonClick, this)));

    mInstance->mYesBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&GameManager::OnYesButtonHoverOn, this)));
    mInstance->mNoBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&GameManager::OnNoButtonHoverOn, this)));
    mInstance->mYesBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&GameManager::OnYesButtonHoverOff, this)));
    mInstance->mNoBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&GameManager::OnNoButtonHoverOff, this)));
}

void GameManager::Update()
{
    mInstance->Controls();
    mInstance->Loading();
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
        mInstance->mPaused = !mPaused;
        mInstance->mPauseScreen->SetEnabled(mPaused);
    }
}

void GameManager::Loading()
{
    if (mInstance->mLoading)
    {
        mInstance->mLoadingScreen->SetEnabled(true);

        if (Delay(0.1f))
        {
            mInstance->mLoading = false;
            App->GetScene()->Load("MainMenu.json");
        }
    }
}

void GameManager::WinScreen() 
{
    mInstance->mWinScreen->SetEnabled(true);
}

void GameManager::LoseScreen() 
{
    mInstance->mLoseScreen->SetEnabled(true);
}

void GameManager::OnWinButtonClick() 
{
    mInstance->mLoading = true;
}

void GameManager::OnLoseButtonClick() 
{
    mInstance->mLoading = true;
}

void GameManager::OnYesButtonClick() 
{
    mInstance->mLoading = true;
}

void GameManager::OnNoButtonClick() 
{
    mInstance->mPaused = false;
    mInstance->mPauseScreen->SetEnabled(false);
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
