#pragma once
#include <Script.h>
#include "Macros.h"

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
    void Menu();
    void ChangeImage(const char* imageName, bool enabled) const;
    void ResetScreen(const char* screenName, bool activeMenu);
    void Loading();
    void Options();
    void Credits();
    bool Delay(float delay);
    void Controls();

    bool mMenuActive = true;
    bool mLoadingActive = false;
    bool mOptionsActive = false;
    bool mCreditsActive = false;

    bool mPauseMenu = false;
    
    bool mNextScreen = false;
    bool mPrevScreen = false;
    int mOption = 1;
    float mTimePassed = 0.0f;
    const char* mActualImageName = nullptr;
    const char* mPreviousImageName = nullptr;
};

