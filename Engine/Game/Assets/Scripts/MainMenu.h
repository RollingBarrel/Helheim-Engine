#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

class ButtonComponent;
class SliderComponent;
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

enum SETTING_TYPE {
	GENERAL_VOLUME,
	MUSIC_VOLUME,
	EFFECTS_VOLUME,
	VSYNC,
	FULL_SCREEN // Using underscore to avoid conflict with fullscreen macro
};

enum DIRECTION {
	LEFT,
	RIGHT
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
    void Controls();

    void HoverMenu(MENU_TYPE type);
    void HoverSubMenu(MENU_TYPE type);
	void HoverSubSubMenu(SETTING_TYPE type);
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
	void OnSlide(SETTING_TYPE type, DIRECTION direction, float step); // step is the increment/decrement amount, usually 0.01f (1%) or 0.1f (10%)


    void OnQuitButtonHover();
    void OnOptionsButtonHover();
    void OnCreditsButtonHover();
    void OnPlayButtonHover();
    void OnBackCreditsButtonHover();
    void OnControlsButtonHover();
    void OnSettingsButtonHover();
    void OnBackButtonHover();
	void OnGeneralVolumeHover();
    void OnMusicVolumeHover();
    void OnEffectsVolumeHover();
    void OnVSyncButtonHover();
    void OnFullscreenButtonHover();

    void OnQuitButtonHoverOff();
    void OnOptionsButtonHoverOff();
    void OnCreditsButtonHoverOff();
    void OnPlayButtonHoverOff();
    void OnBackCreditsButtonHoverOff();
    void OnControlsButtonHoverOff();
    void OnSettingsButtonHoverOff();
    void OnBackButtonHoverOff();
    void OnGeneralVolumeHoverOff();
    void OnMusicVolumeHoverOff();
    void OnEffectsVolumeHoverOff();
    void OnVSyncButtonHoverOff();
    void OnFullscreenButtonHoverOff();
    
    int mOption = 0;
    int mSettingOption = 7;
    int mSubsettingOption = 0;

    bool mLoadlevel = false;

    float mTimePassed = 0.0f;

    MENU_TYPE mCurrentMenu;
	SETTING_TYPE mCurrentSetting;

    float mGeneralVolumeValue = .75f;
    float mMusicVolumeValue = .75f;
    float mEffectsVolumeValue = .75f;

    TimerScript mTimer;
    TimerScript mStudioTimer;
    bool mStudioBool = true;
    TimerScript mEngineTimer;
    bool mEngineBool = true;

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

    // Options
    GameObject* mOptionsContainerGO = nullptr;
    GameObject* mSettingsButtonGO = nullptr;
    GameObject* mSettingsGO = nullptr;
    ButtonComponent* mSettingsButton = nullptr;
    GameObject* mControlsGO = nullptr;
    GameObject* mControlsButtonGO = nullptr;
    ButtonComponent* mControlsButton = nullptr;

    // Settings
    // General Volume
    GameObject* mGeneralVolumeSliderGO = nullptr;
    SliderComponent* mGeneralVolumeSlider = nullptr;
    GameObject* mGeneralVolumeFillGO = nullptr;
    ImageComponent* mGeneralVolumeFill = nullptr;
    // Music Volume
    GameObject* mMusicVolumeSliderGO = nullptr;
    SliderComponent* mMusicVolumeSlider = nullptr;
    GameObject* mMusicVolumeFillGO = nullptr;
    ImageComponent* mMusicVolumeFill = nullptr;
	// Effects Volume
    GameObject* mEffectsVolumeSliderGO = nullptr;
    SliderComponent* mEffectsVolumeSlider = nullptr;
    GameObject* mEffectsVolumeFillGO = nullptr;
    ImageComponent* mEffectsVolumeFill = nullptr;
    // VSync
    GameObject* mVSyncButtonGO = nullptr;
    ButtonComponent* mVSyncButton = nullptr;
    ImageComponent* mVSyncImage = nullptr;
	// Fullscreen
    GameObject* mFullscreenButtonGO = nullptr;
    ImageComponent* mFullscreenImage = nullptr;
    ButtonComponent* mFullscreenButton = nullptr;

    GameObject* mAudioManagerGO = nullptr;
    AudioManager* mAudioManager = nullptr;
    int mBGMID = -1;

    bool mIsScrolling = false;
    bool mIsInitial = true;
};

