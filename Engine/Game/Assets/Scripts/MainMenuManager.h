#pragma once
#include <Script.h>
#include "Macros.h"

class AudioSourceComponent;

GENERATE_BODY(MainMenuController);

class MainMenuController :
    public Script
{
    FRIEND(MainMenuController)

public:
    MainMenuController(GameObject* owner);
    ~MainMenuController();

    void PlaySFX();

    void Start() override;
    void Update() override;

private:
    GameObject* mMainMenuThemeHolder = nullptr;
    AudioSourceComponent* mMainMenuTheme = nullptr;

    GameObject* mSFXHolder = nullptr;
    AudioSourceComponent* mSFX = nullptr;
};

