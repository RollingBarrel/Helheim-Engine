#pragma once
#include <Script.h>
#include "Macros.h"

class ButtonComponent;

enum MENU_TYPE {
    MAIN,
    OPTIONS,
    CREDITS,
    LOADING,
    SPLASH
};

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

    void OpenMenu(MENU_TYPE type);
    void OnMainButtonClick();
    void OnQuitButtonClick();
    void OnOptionsButtonClick();
    void OnCreditsButtonClick();
    void OnNewButtonClick();
    void OnSplashButtonClick();

    void OnQuitButtonHover();
    void OnOptionsButtonHover();
    void OnCreditsButtonHover();
    void OnNewButtonHover();
    void OnBackCreditsButtonHover();
    void OnQuitButtonHoverOff();
    void OnOptionsButtonHoverOff();
    void OnCreditsButtonHoverOff();
    void OnNewButtonHoverOff();
    void OnBackCreditsButtonHoverOff();

    bool mMenuActive = true;
    bool mPauseMenu = false;

    bool mLoadingActive = false;
    bool mOptionsActive = false;
    bool mCreditsActive = false;
    bool mNextScreen = false;
    bool mPrevScreen = false;
    bool mReturnPressed = false;
    bool mEscPressed = false;
    
    int mOptionTmp = mOption;
    float mTimePassed = 0.0f;
    const char* mScreen = nullptr;
    const char* mActualImageName = nullptr;
    const char* mPreviousImageName = nullptr;
    
    int mOption = 0;  
    bool mLoadlevel = false;

    GameObject* mMainMenu = nullptr;
    GameObject* mOptionsMenu = nullptr;
    GameObject* mCreditsMenu = nullptr;
    GameObject* mLoadingMenu = nullptr;
    GameObject* mSplashScreen = nullptr;

    GameObject* mContainerGO = nullptr;
    GameObject* mSplashGO = nullptr;
    GameObject* mNewGO = nullptr;
    GameObject* mOptionsGO = nullptr;
    GameObject* mCreditsGO = nullptr;
    GameObject* mQuitGO = nullptr;
    GameObject* mBackCreditGO = nullptr;
    GameObject* mOptionsContainerGO = nullptr;

    ButtonComponent* mSplashButton = nullptr;
    ButtonComponent* mNewButton = nullptr;
    ButtonComponent* mOptionsButton = nullptr;
    ButtonComponent* mCreditsButton = nullptr;
    ButtonComponent* mQuitButton = nullptr;
    ButtonComponent* mBackCreditButton = nullptr;
};

