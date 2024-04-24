#pragma once
#include <Script.h>
#include "Macros.h"

class MainMenuManager;

GENERATE_BODY(MainMenu);

class MainMenu : public Script
{
    FRIEND(MainMenu)
public:
    MainMenu(GameObject* owner);
    ~MainMenu() {}
    void Start() override;
    void Update() override;
    void SetMenu(bool active, bool pause);

private:
    void ButtonsPosition(const char* imageName, float position) const;
    void Menu();
    void ChangeImage(const char* imageName, bool enabled) const;
    void ResetScreen(const char* screenName, bool activeMenu);
    void Loading();
    void Options();
    void Credits();
    bool Delay(float delay);
    void Controls();

    bool mMenuActive = true;
    bool mPauseMenu = false;

    bool mLoadingActive = false;
    bool mOptionsActive = false;
    bool mCreditsActive = false;
    bool mNextScreen = false;
    bool mPrevScreen = false;
    bool mReturnPressed = false;
    bool mEscPressed = false;
    int mOption = 2;  
    int mOptionTmp = mOption;
    float mTimePassed = 0.0f;
    const char* mScreen = nullptr;
    const char* mActualImageName = nullptr;
    const char* mPreviousImageName = nullptr;

    GameObject* mMainMenuManagerHolder = nullptr;
    MainMenuManager* mMainMenuManager = nullptr;
};

