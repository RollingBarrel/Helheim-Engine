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
    KEYBOARD,
    CONTROLS,
    AUDIO,
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
    void OnPlayButtonClick();
    void OnSplashButtonClick();
    void OnControllerButtonClick();
    void OnKeyboardButtonClick();
    void OnAudioButtonClick();
    void OnSettingsButtonClick();
    void OnVSyncButtonOnClick();
    void OnVSyncButtonOffClick();
    void OnFullscreenButtonOnClick();
    void OnFullscreenButtonOffClick();
	void OnSlide(SETTING_TYPE type, DIRECTION direction, float step); // step is the increment/decrement amount, usually 0.01f (1%) or 0.1f (10%)


    void OnQuitButtonHover();
    void OnOptionsButtonHover();
    void OnCreditsButtonHover();
    void OnPlayButtonHover();
    void OnControllerButtonHover();
    void OnKeyboardButtonHover();
    void OnAudioButtonHover();
    void OnSettingsButtonHover();
	void OnGeneralVolumeHover();
    void OnMusicVolumeHover();
    void OnEffectsVolumeHover();
    //void OnVSyncButtonHover();
    //void OnFullscreenButtonHover();

    void OnQuitButtonHoverOff();
    void OnOptionsButtonHoverOff();
    void OnCreditsButtonHoverOff();
    void OnPlayButtonHoverOff();
    void OnControllerButtonHoverOff();
    void OnKeyboardButtonHoverOff();
    void OnAudioButtonHoverOff();
    void OnSettingsButtonHoverOff();
    void OnGeneralVolumeHoverOff();
    void OnMusicVolumeHoverOff();
    void OnEffectsVolumeHoverOff();
    //void OnVSyncButtonHoverOff();
    //void OnFullscreenButtonHoverOff();

    void OnGeneralUp();
    void OnGeneralDown();
    void OnMusicUp();
    void OnMusicDown();
    void OnEffectsUp();
    void OnEffectsDown();
    
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
    GameObject* mLoadingSliderGO = nullptr;
    SliderComponent* mLoadingSlider = nullptr;

    GameObject* mMainMenu = nullptr;
    GameObject* mOptionsMenu = nullptr;
    GameObject* mCreditsMenu = nullptr;
    GameObject* mCreditsListText = nullptr;
    Transform2DComponent* mTextTransform = nullptr;

    // --- BUTTON GameObjects ---
    
    // PLAY Button
    GameObject* mPlayGO = nullptr;
    GameObject* mPlayText = nullptr;
    GameObject* mPlayHover = nullptr;
    GameObject* mPlayClicked = nullptr;
    ButtonComponent* mPlayButton = nullptr;

    // OPTIONS Button
    GameObject* mOptionsGO = nullptr;
    GameObject* mOptionsText = nullptr;
    GameObject* mOptionsHover = nullptr;
    GameObject* mOptionsClicked = nullptr;
    ButtonComponent* mOptionsButton = nullptr;

    // Credits Button
    GameObject* mCreditsGO = nullptr;
    GameObject* mCreditsText = nullptr;
    GameObject* mCreditsHover = nullptr;
    GameObject* mCreditsClicked = nullptr;
    ButtonComponent* mCreditsButton = nullptr;

    // OPTIONS Button
    GameObject* mQuitGO = nullptr;
    GameObject* mQuitText = nullptr;
    GameObject* mQuitHover = nullptr;
    GameObject* mQuitClicked = nullptr;
    ButtonComponent* mQuitButton = nullptr;

    // Keyboard Button
    GameObject* mKeyboardGO = nullptr;
    GameObject* mKeyboardText = nullptr;
    GameObject* mKeyboardHover = nullptr;
    GameObject* mKeyboardClicked = nullptr;
    ButtonComponent* mKeyboardButton = nullptr;

    // Controller Button
    GameObject* mControllerGO = nullptr;
    GameObject* mControllerText = nullptr;
    GameObject* mControllerHover = nullptr;
    GameObject* mControllerClicked = nullptr;
    ButtonComponent* mControllerButton = nullptr;

    // Audio Button
    GameObject* mAudioGO = nullptr;
    GameObject* mAudioText = nullptr;
    GameObject* mAudioHover = nullptr;
    GameObject* mAudioClicked = nullptr;
    ButtonComponent* mAudioButton = nullptr;

    // Settings Button
    GameObject* mSettingsGO = nullptr;
    GameObject* mSettingsText = nullptr;
    GameObject* mSettingsHover = nullptr;
    GameObject* mSettingsClicked = nullptr;
    ButtonComponent* mSettingsButton = nullptr;

    // Options
    GameObject* mOptionsContainerGO = nullptr;
    GameObject* mSettingsScreenGO = nullptr;
    GameObject* mControlsScreenGO = nullptr;
    GameObject* mAudioScreenGO = nullptr;
    GameObject* mKeyboardScreenGO = nullptr;

    // Settings
    // General Volume
    GameObject* mGeneralVolumeSliderGO = nullptr;
    SliderComponent* mGeneralVolumeSlider = nullptr;
    GameObject* mGeneralVolumeFillGO = nullptr;
    ImageComponent* mGeneralVolumeFill = nullptr;
    GameObject* mGeneralVolumeUp = nullptr;
    GameObject* mGeneralVolumeDown = nullptr;

    // Music Volume
    GameObject* mMusicVolumeSliderGO = nullptr;
    SliderComponent* mMusicVolumeSlider = nullptr;
    GameObject* mMusicVolumeFillGO = nullptr;
    ImageComponent* mMusicVolumeFill = nullptr;
    GameObject* mMusicVolumeUp = nullptr;
    GameObject* mMusicVolumeDown = nullptr;

	// Effects Volume
    GameObject* mEffectsVolumeSliderGO = nullptr;
    SliderComponent* mEffectsVolumeSlider = nullptr;
    GameObject* mEffectsVolumeFillGO = nullptr;
    ImageComponent* mEffectsVolumeFill = nullptr;
    GameObject* mEffectsVolumeUp = nullptr;
    GameObject* mEffectsVolumeDown = nullptr;

    // VSync
    GameObject* mVSyncButtonOnGO = nullptr;
    GameObject* mVSyncButtonOffGO = nullptr;
    GameObject* mVSyncImageOn_On = nullptr;
    GameObject* mVSyncImageOff_On = nullptr;
    ButtonComponent* mVSyncOnButton = nullptr;
    ButtonComponent* mVSyncOffButton = nullptr;
	// Fullscreen
    GameObject* mFullscreenButtonOnGO = nullptr;
    GameObject* mFullscreenButtonOffGO = nullptr;
    ButtonComponent* mFullscreenOnButton = nullptr;
    ButtonComponent* mFullscreenOffButton = nullptr;
    GameObject* mFullscreenImageOn_On = nullptr;
    GameObject* mFullscreenImageOff_On = nullptr;

    GameObject* mAudioManagerGO = nullptr;
    AudioManager* mAudioManager = nullptr;
    int mBGMID = -1;

    bool mIsScrolling = false;
    bool mIsInitial = true;
};

