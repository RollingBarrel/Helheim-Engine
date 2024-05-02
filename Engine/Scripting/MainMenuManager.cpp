#include "MainMenuManager.h"
#include "AudioSourceComponent.h"
#include "GameObject.h"

CREATE(MainMenuManager)
{
    CLASS(owner);
    MEMBER(MemberType::GAMEOBJECT, mMainMenuThemeHolder);
    MEMBER(MemberType::GAMEOBJECT, mOKSFXHolder);
    MEMBER(MemberType::GAMEOBJECT, mSelectSFXHolder);
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
        mMainMenuTheme->Play();
    }

    if (mOKSFXHolder != nullptr) 
    {
        mOKSFX = (AudioSourceComponent*)mOKSFXHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }

    if (mSelectSFXHolder != nullptr) 
    {
        mSelectSFX = (AudioSourceComponent*)mSelectSFXHolder->GetComponent(ComponentType::AUDIOSOURCE);
    }
}

void MainMenuManager::Update()
{
}

void MainMenuManager::PlayOKSFX()
{
    mOKSFX->PlayOneShot();
}

void MainMenuManager::PlaySelectSFX()
{
    mSelectSFX->PlayOneShot();
}