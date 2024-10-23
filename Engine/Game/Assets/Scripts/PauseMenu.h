#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

class ButtonComponent;
class SliderComponent;
class Transform2DComponent;
class ImageComponent;
class AudioManager;

enum MENU_TYPE 
{
    MAIN,
    OPTIONS,
    CREDITS,
    QUIT,
    KEYBOARD,
    CONTROLS,
    AUDIO_SETTINGS,
    VIDEO_SETTINGS
};

enum AUDIO_SETTING_TYPE 
{
    MASTER,
    MUSIC,
    EFFECTS
};

enum VIDEO_SETTING_TYPE 
{
    VSYNC,
    FULL_SCREEN
};

enum DIRECTION 
{
    LEFT,
    RIGHT
};

GENERATE_BODY(PauseMenu);

class PauseMenu : public Script
{
    FRIEND(PauseMenu)
public:
    PauseMenu(GameObject* owner);
    ~PauseMenu() {}
    void Start() override;
    void Update() override;

    void Reset();

private:
    void Controls();

    void HoverMenu(MENU_TYPE type);
    void HoverVideoMenu(VIDEO_SETTING_TYPE type);
    void HoverAudioMenu(AUDIO_SETTING_TYPE type);
    void ClickMenu(MENU_TYPE type);
    void OpenMenu(MENU_TYPE type);

    void OnMainButtonClick();
    void OnQuitButtonClick();
    void OnOptionsButtonClick();
    void OnCreditsButtonClick();
    void OnBackButtonClick();
    void OnPlayButtonClick();
    void OnControllerButtonClick();
    void OnKeyboardButtonClick();
    void OnAudioButtonClick();
    void OnSettingsButtonClick();
    void OnVSyncButtonOnClick();
    void OnVSyncButtonOffClick();
    void OnFullscreenButtonOnClick();
    void OnFullscreenButtonOffClick();
    void OnVolumeSlide(AUDIO_SETTING_TYPE type, DIRECTION direction, float step); // step is the increment/decrement amount, usually 0.01f (1%) or 0.1f (10%)

    void OnQuitButtonHover();
    void OnOptionsButtonHover();
    void OnCreditsButtonHover();
    void OnPlayButtonHover();
    void OnControllerButtonHover();
    void OnKeyboardButtonHover();
    void OnAudioSettingsButtonHover();
    void OnVideoSettingsButtonHover();
    void OnGeneralVolumeHover();
    void OnMusicVolumeHover();
    void OnEffectsVolumeHover();
    void OnVSyncButtonHover();
    void OnFullscreenButtonHover();

    void OnQuitButtonHoverOff();
    void OnOptionsButtonHoverOff();
    void OnCreditsButtonHoverOff();
    void OnPlayButtonHoverOff();
    void OnControllerButtonHoverOff();
    void OnKeyboardButtonHoverOff();
    void OnAudioSettingsButtonHoverOff();
    void OnVideoSettingsButtonHoverOff();
    void OnGeneralVolumeHoverOff();
    void OnMusicVolumeHoverOff();
    void OnEffectsVolumeHoverOff();
    void OnVSyncButtonHoverOff();
    void OnFullscreenButtonHoverOff();

    void OnGeneralUp();
    void OnGeneralDown();
    void OnMusicUp();
    void OnMusicDown();
    void OnEffectsUp();
    void OnEffectsDown();

    int mMainOption = 0;
    int mOptionsOption = 4;
    int mVideoSettingOption = 0;
    int mAudioSettingOption = 0;

    float mTimePassed = 0.0f;
    const float mDebounceTime = 0.2f; // 200 ms delay time

    MENU_TYPE mCurrentMenu;
    AUDIO_SETTING_TYPE mCurrentAudioSetting;
    VIDEO_SETTING_TYPE mCurrentVideoSetting;

    float mMasterVolumeValue = .75f;
    float mMusicVolumeValue = .75f;
    float mEffectsVolumeValue = .75f;

    bool mIsAdjustingAudio = false;

    TimerScript mTimer;

    // Screens
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
    bool mIsScrolling = false;

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

    // Back Button
    GameObject* mBackGO = nullptr;
    ButtonComponent* mBackButton = nullptr;

    // Settings
    // General Volume
    GameObject* mGeneralVolumeSliderGO = nullptr;
    SliderComponent* mGeneralVolumeSlider = nullptr;
    GameObject* mGeneralVolumeBackgroundGO = nullptr;
    GameObject* mGeneralVolumeFillGO = nullptr;
    ImageComponent* mGeneralVolumeFill = nullptr;
    GameObject* mGeneralVolumeUp = nullptr;
    GameObject* mGeneralVolumeDown = nullptr;

    // Music Volume
    GameObject* mMusicVolumeSliderGO = nullptr;
    SliderComponent* mMusicVolumeSlider = nullptr;
    GameObject* mMusicVolumeBackgroundGO = nullptr;
    GameObject* mMusicVolumeFillGO = nullptr;
    ImageComponent* mMusicVolumeFill = nullptr;
    GameObject* mMusicVolumeUp = nullptr;
    GameObject* mMusicVolumeDown = nullptr;

    // Effects Volume
    GameObject* mEffectsVolumeSliderGO = nullptr;
    SliderComponent* mEffectsVolumeSlider = nullptr;
    GameObject* mEffectsVolumeBackgroundGO = nullptr;
    GameObject* mEffectsVolumeFillGO = nullptr;
    ImageComponent* mEffectsVolumeFill = nullptr;
    GameObject* mEffectsVolumeUp = nullptr;
    GameObject* mEffectsVolumeDown = nullptr;

    // VSync
    GameObject* mVSyncBackgroundGO = nullptr;
    GameObject* mVSyncButtonOnGO = nullptr;
    GameObject* mVSyncButtonOffGO = nullptr;
    GameObject* mVSyncImageOn_On = nullptr;
    GameObject* mVSyncImageOff_On = nullptr;
    ButtonComponent* mVSyncOnButton = nullptr;
    ButtonComponent* mVSyncOffButton = nullptr;
    // Fullscreen
    GameObject* mFullscreenBackgroundGO = nullptr;
    GameObject* mFullscreenButtonOnGO = nullptr;
    GameObject* mFullscreenButtonOffGO = nullptr;
    ButtonComponent* mFullscreenOnButton = nullptr;
    ButtonComponent* mFullscreenOffButton = nullptr;
    GameObject* mFullscreenImageOn_On = nullptr;
    GameObject* mFullscreenImageOff_On = nullptr;

    AudioManager* mAudioManager = nullptr;
    int mBGMID = -1;
};

