#pragma once
#include "Script.h"
#include "Macros.h"

class ButtonComponent;
class Transform2DComponent;
class ImageComponent;
class AudioManager;

enum MENU_TYPE {
    MAIN,
    OPTIONS,
    CREDITS,
    LOADING,
    SPLASH,
    ENGINE,
    STUDIO,
    CONTROLS,
    SETTINGS
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

private:
    
    bool Delay(float delay);
    void Controls();

    void HoverMenu(MENU_TYPE type);
    void HoverSubMenu(int type);
    void ClickMenu(MENU_TYPE type);
    void OpenMenu(MENU_TYPE type);

    void OnMainButtonClick();
    void OnQuitButtonClick();
    void OnOptionsButtonClick();
    void OnCreditsButtonClick();
    void OnNewButtonClick();
    void OnSplashButtonClick();
    void OnControlsButtonClick();
    void OnSettingsButtonClick();
    void OnVSyncButtonClick();
    void OnFullscreenButtonClick();

    void OnQuitButtonHover();
    void OnOptionsButtonHover();
    void OnCreditsButtonHover();
    void OnPlayButtonHover();
    void OnBackCreditsButtonHover();
    void OnControlsButtonHover();
    void OnSettingsButtonHover();
    void OnBackButtonHover();

    void OnQuitButtonHoverOff();
    void OnOptionsButtonHoverOff();
    void OnCreditsButtonHoverOff();
    void OnPlayButtonHoverOff();
    void OnBackCreditsButtonHoverOff();
    void OnControlsButtonHoverOff();
    void OnSettingsButtonHoverOff();
    void OnBackButtonHoverOff();
    
    int mOption = 0;
    int mSettingOption = 0;
    int mSection = 0;

    bool mLoadlevel = false;

    float mTimePassed = 0.0f;
    MENU_TYPE mCurrentMenu;

    // Screens
    GameObject* mSplashScreen = nullptr;
    ButtonComponent* mSplashButton = nullptr;
    GameObject* mStudioScreen = nullptr;
    GameObject* mEngineScreen = nullptr;
    GameObject* mLoadingScreen = nullptr;

    GameObject* mMainMenu = nullptr;
    GameObject* mOptionsMenu = nullptr;
    GameObject* mCreditsMenu = nullptr;
    GameObject* mCreditsText = nullptr;
    Transform2DComponent* mTextTransform = nullptr;

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

    GameObject* mOptionsContainerGO = nullptr;
    GameObject* mSettingsButtonGO = nullptr;
    GameObject* mSettingsGO = nullptr;
    ButtonComponent* mSettingsButton = nullptr;
    GameObject* mControlsGO = nullptr;
    GameObject* mControlsButtonGO = nullptr;
    ButtonComponent* mControlsButton = nullptr;

    GameObject* mGeneralVolumeButtonGO = nullptr;
    ButtonComponent* mGeneralVolumeButton = nullptr;
    GameObject* mMusicVolumeButtonGO = nullptr;
    ButtonComponent* mMusicVolumeButton = nullptr;
    GameObject* mEffectsVolumeButtonGO = nullptr;
    ButtonComponent* mEffectsVolumeButton = nullptr;
    GameObject* mVSyncButtonGO = nullptr;
    ButtonComponent* mVSyncButton = nullptr;
    ImageComponent* mVSyncImage = nullptr;
    GameObject* mFullscreenButtonGO = nullptr;
    ImageComponent* mFullscreenImage = nullptr;
    ButtonComponent* mFullscreenButton = nullptr;

    GameObject* mAudioManagerGO = nullptr;
    AudioManager* mAudioManager = nullptr;
    int mBGMID = -1;

    bool mIsScrolling = false;
    bool mIsInitial = true;
};

