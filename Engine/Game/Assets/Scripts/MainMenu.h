#pragma once
#include "Script.h"
#include "Macros.h"

class MainMenuManager;
class ButtonComponent;

enum MENU_TYPE {
    MAIN,
    OPTIONS,
    CREDITS,
    LOADING,
    SPLASH,
    ENGINE,
    STUDIO
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
    void HoverMenu(MENU_TYPE type);
    void ClickMenu(MENU_TYPE type);

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
    void OnPlayButtonHover();
    void OnBackCreditsButtonHover();
    void OnQuitButtonHoverOff();
    void OnOptionsButtonHoverOff();
    void OnCreditsButtonHoverOff();
    void OnPlayButtonHoverOff();
    void OnBackCreditsButtonHoverOff();

    //bool mMenuActive = true;
    //bool mPauseMenu = false;

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

    GameObject* mMainMenuManagerHolder = nullptr;
    MainMenuManager* mMainMenuManager = nullptr;
    
    int mOption = 0;  
    bool mLoadlevel = false;

    // Screens
    GameObject* mSplashScreen = nullptr;
    ButtonComponent* mSplashButton = nullptr;
    GameObject* mStudioScreen = nullptr;
    ButtonComponent* mStudioButton = nullptr;
    GameObject* mEngineScreen = nullptr;
    ButtonComponent* mEngineButton = nullptr;
    GameObject* mLoadingScreen = nullptr;

    GameObject* mMainMenu = nullptr;
    GameObject* mOptionsMenu = nullptr;
    GameObject* mCreditsMenu = nullptr;

    GameObject* mPlayGO = nullptr;
    ButtonComponent* mPlayButton = nullptr;
    GameObject* mOptionsGO = nullptr;
    ButtonComponent* mOptionsButton = nullptr;
    GameObject* mCreditsGO = nullptr;
    ButtonComponent* mCreditsButton = nullptr;
    GameObject* mQuitGO = nullptr;
    ButtonComponent* mQuitButton = nullptr;
    GameObject* mBackCreditGO = nullptr;
    ButtonComponent* mBackCreditButton = nullptr;

    //GameObject* mContainerGO = nullptr;
    //GameObject* mOptionsContainerGO = nullptr;

    // Scrolling related variables
    //float3 mStartPosition;
    //float3 mEndPosition;
    //float mScrollDuration;
    //float mElapsedTime;
    //bool mIsScrolling;
};

