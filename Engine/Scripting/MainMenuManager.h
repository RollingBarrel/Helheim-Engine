#pragma once
#include "Script.h"
#include "Macros.h"

class AudioSourceComponent;

GENERATE_BODY(MainMenuManager);

class MainMenuManager :
    public Script
{
    FRIEND(MainMenuManager)

public:
    MainMenuManager(GameObject* owner);
    ~MainMenuManager();

    void PlayOKSFX();
    void PlaySelectSFX();

    void Start() override;
    void Update() override;

private:
    GameObject* mMainMenuThemeHolder = nullptr;
    AudioSourceComponent* mMainMenuTheme = nullptr;
};

