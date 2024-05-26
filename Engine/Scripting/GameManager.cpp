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
    SEPARATOR("Player");
    MEMBER(MemberType::GAMEOBJECT, mPlayer);
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

void GameManager::Awake()
{
    mInstance = this;
}

void GameManager::Start()
{
    mPauseScreen->SetEnabled(false);
    mWinScreen->SetEnabled(false);
    mLoseScreen->SetEnabled(false);
    mLoadingScreen->SetEnabled(false);

    mWinBtn = static_cast<ButtonComponent*>(mWinScreen->GetComponent(ComponentType::BUTTON));
    mLoseBtn = static_cast<ButtonComponent*>(mLoseScreen->GetComponent(ComponentType::BUTTON));
    mYesBtn = static_cast<ButtonComponent*>(mYesGO->GetComponent(ComponentType::BUTTON));
    mNoBtn = static_cast<ButtonComponent*>(mNoGO->GetComponent(ComponentType::BUTTON));

    
    mWinBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&GameManager::OnWinButtonClick, this)));
    mLoseBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&GameManager::OnLoseButtonClick, this)));
    mYesBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&GameManager::OnYesButtonClick, this)));
    mNoBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&GameManager::OnNoButtonClick, this)));

    mYesBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&GameManager::OnYesButtonHoverOn, this)));
    mNoBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&GameManager::OnNoButtonHoverOn, this)));
    mYesBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&GameManager::OnYesButtonHoverOff, this)));
    mNoBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&GameManager::OnNoButtonHoverOff, this)));
}

void GameManager::Update()
{
     Controls();
     Loading();
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

void GameManager::Loading()
{
    if ( mLoading)
    {
        mLoadingScreen->SetEnabled(true);

        if (Delay(0.1f))
        {
            mLoading = false;
            LoadLevel("MainMenu.json");
        }
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

void GameManager::LoadLevel(const char* LevelName)
{
    App->GetScene()->Load(LevelName);
}

void GameManager::OnWinButtonClick() 
{
    mLoading = true;
}

void GameManager::OnLoseButtonClick() 
{
    mLoading = true;
}

void GameManager::OnYesButtonClick() 
{
    mLoading = true;
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
