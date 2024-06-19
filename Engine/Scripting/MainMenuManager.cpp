#include "MainMenuManager.h"
#include "GameManager.h"
#include "AudioManager.h"
#include "AudioSourceComponent.h"
#include "GameObject.h"

CREATE(MainMenuManager)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mMainMenuThemeHolder);
    END_CREATE;
}

MainMenuManager::MainMenuManager(GameObject* owner) : Script(owner)
{
}

MainMenuManager::~MainMenuManager()
{
}

void MainMenuManager::Start()
{
    if (mMainMenuThemeHolder != nullptr) 
    {
        mMainMenuTheme = (AudioSourceComponent*)mMainMenuThemeHolder->GetComponent(ComponentType::AUDIOSOURCE);
        //mMainMenuTheme->Play();
    }
}

void MainMenuManager::Update()
{
}

void MainMenuManager::PlayOKSFX()
{
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_OK);
}

void MainMenuManager::PlaySelectSFX()
{
    GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_SELECT);
}