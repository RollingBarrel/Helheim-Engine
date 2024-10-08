#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

class ButtonComponent;
class SliderComponent;
class TextComponent;
class ImageComponent;
class VideoComponent;
class PlayerController;
class Sanity;
class Dialog;
class Transform2DComponent;
class PauseMenu;
enum class EnergyType;

enum SCREEN {
    LOAD,
    LOSE,
    WIN,
    PAUSE,
    COLLECTIBLE
};

enum class LoseOption {
    LOSE_TRY_AGAIN,
    LOSE_MENU
};

GENERATE_BODY(HudController);

class HudController : public Script
{
    FRIEND(HudController)
public:
    void Start();
    void Update();

    void SetAmmo(int ammo);
    void SetEnergy(int energy, EnergyType type, bool up);
    void SetHealth(float health);
    void SetBossHealth(float health);
    void SetBossInvulnerable(bool value);
    void SetBossHealthBarEnabled(bool enabled);
    void SetMaxHealth(float health);
    void SetGrenadeCooldown(float cooldown);
    void SetUltimateCooldown(float cooldown);
    void SetCollectibleText(std::string text, std::string title, std::string subtitle);
    void SetScreen(SCREEN menu, bool active);
    void SetInteract(bool active);
    void SetPickupPrompt(bool active);

    void SetSanity();
    void SetDialog();
    void DisableCollectible();
    void SetFadein(bool value) { mFadeIn = value; }

    void SetEnemyScreen(bool value, int enemy);

    void SetGodmode(bool value);
    void SetInstaKill(bool value);
    void SetDialogue(bool value);
    void SetDebug(bool value);

    void ChangeBindings(bool controller);
    void SetHud(bool value);
private:
    HudController(GameObject* owner);
    ~HudController();

    void Controls();
    bool Delay(float delay);
    void LoseUpdate();
    void WinUpdate();

    void OnWinButtonClick();
    void OnTryAgainButtonClick();
    void OnVideoBackClick();
    void OnTryAgainButtonHoverOn();
    void OnTryAgainButtonHoverOff();
    void OnLoseButtonClick();
    void OnLoseButtonHoverOn();
    void OnLoseButtonHoverOff();
	void OnSelectLoseOption(LoseOption option);

    void OnCollectibleContinueBtnClick();
    void OnCollectibleContinueBtnHoverOn();
    void OnCollectibleContinueBtnHoverOff();

    void FadeIn();
    void FadeOut();

    void LoseAnimation();
    void WinAnimation();
    
    void PlayVideoAssociatedAudio();
    void ReleaseVideoAssociatedAudio();

    // Pause Menu
    GameObject* mPauseScreen = nullptr;
    PauseMenu* mPauseMenu = nullptr;
    GameObject* mFadeoutScreen = nullptr;
    ImageComponent* mFadeoutImage = nullptr;

    GameObject* mWinScreen = nullptr;
    GameObject* mLoseScreen = nullptr;
    GameObject* mLoadingScreen = nullptr;
    GameObject* mLoadingSliderGO = nullptr;
    SliderComponent* mLoadingSlider = nullptr;
    GameObject* mHealthGO = nullptr;
    GameObject* mHealthGradualGO = nullptr;
    GameObject* mHealthIconGO = nullptr;
    GameObject* mAmmoGO = nullptr;
    GameObject* mWeaponRangeGO = nullptr;
    GameObject* mGrenadeSliderGO = nullptr;
    GameObject* mUltimateSliderGO = nullptr;
    GameObject* mFeedbackGO = nullptr;
    GameObject* mCollectibleScreen = nullptr;
    GameObject* mCollectibleTextGO = nullptr;
    GameObject* mCollectibleTitleGO = nullptr;
    GameObject* mCollectibleSubtitleGO = nullptr;
    GameObject* mCollectibleImageGO = nullptr;
    GameObject* mCollectibleContinueBtnGO = nullptr;
    GameObject* mInteractGO = nullptr;
    GameObject* mPickupControllerGO = nullptr;
    GameObject* mPickupKeyboardGO = nullptr;

    GameObject* mTryAgainBtnGO = nullptr;
    GameObject* mTryAgainBtnClicked = nullptr;
    GameObject* mLoseMenuBtnGO = nullptr;
    GameObject* mLoseMenuBtnClicked = nullptr;
    GameObject* mWinMenuBtnGO = nullptr;
    ButtonComponent* mLoseBtn = nullptr;
    ButtonComponent* mWinBtn = nullptr;
    ButtonComponent* mTryAgainBtn = nullptr;

    SliderComponent* mHealthSlider = nullptr;
    SliderComponent* mHealthGradualSlider = nullptr;
    ImageComponent* mHealthImage = nullptr;
    ImageComponent* mHealthIcon = nullptr;
    ImageComponent* mHealthGradualImage = nullptr;
    TextComponent* mAmmoText = nullptr;
    SliderComponent* mGrenadeSlider = nullptr;
    SliderComponent* mUltimateSlider = nullptr;
    ImageComponent* mFeedbackImage = nullptr;
    TextComponent* mLoreText = nullptr;
    TextComponent* mTitleText = nullptr;
    TextComponent* mSubtitleText = nullptr;
    TextComponent* mInteractText = nullptr;
    ButtonComponent* mCollectibleContinueBtn = nullptr;

    //Debug
    GameObject* mDebugGO = nullptr;
    GameObject* mGodmodeGO = nullptr;
    GameObject* mInstakillGO = nullptr;
    GameObject* mDialogueGO = nullptr;
    ImageComponent* mGodmodeImage = nullptr;
    ImageComponent* mInstakillImage = nullptr;
    ImageComponent* mDialogueImage = nullptr;

    //Boss Health bar
    GameObject* mBossHealthGO = nullptr;
    GameObject* mBossHealthGradualGO = nullptr;
    float mBossHealth = 0.0;
    SliderComponent* mBossHealthSlider = nullptr;
    ImageComponent* mBossHealthImage = nullptr;
    SliderComponent* mBossHealthGradualSlider = nullptr;

    //Ammo
    GameObject* mAmmoBaseGO = nullptr;
    GameObject* mAmmoShotgunGO = nullptr;
    GameObject* mAmmoSubGO = nullptr;

    float mTargetHealth = 1;

    bool mPaused = false;
    float mTimeScreen = 5.0f;
    float mTimePassed = 0.0f;
    bool mFadeIn = true;
    bool mLoadlevel = false;

    GameObject* mMenuScript = nullptr;

    float mGrenadeCooldown = 0.0f;
    float mGrenadeTimer = 0.0f;
    float mUltimateCooldown = 0.0f;
    float mUltimateTimer = 0.0f;

    // Sanity
    GameObject* mSanityGO = nullptr;
    Sanity* mSanity = nullptr;

    // Dialog
    GameObject* mDialogGO = nullptr;
    Dialog* mDialog = nullptr;

    int mArenaCounter = 0;

    // Debounce gamepad analog input (joystick)
    float mDebounceTimePassed = 0.0f;
    const float mDebounceTime = 0.2f; // 200 ms delay time

    // Lose Screen
	LoseOption mLoseOption = LoseOption::LOSE_TRY_AGAIN;

    // Lose Animation
    TimerScript mLoseAnimationTimer;
    bool mLoseFlag = false;
    GameObject* mLoseBackText = nullptr;
    ImageComponent* mLoseBackImage = nullptr;
    GameObject* mLoseFade = nullptr;
    ImageComponent* mLoseFadeImage = nullptr;
    GameObject* mLoseText = nullptr;
    ImageComponent* mLoseTextImage = nullptr;
    GameObject* mLoseLineRight = nullptr;
    Transform2DComponent* mLoseLineRightTransfrom = nullptr;
    GameObject* mLoseLineLeft = nullptr;
    Transform2DComponent* mLoseLineLeftTransfrom = nullptr;

    // Win Animation
    TimerScript mWinAnimationTimer;
    bool mWinFlag = false;
    GameObject* mWinBackText = nullptr;
    ImageComponent* mWinBackImage = nullptr;
    GameObject* mWinFade = nullptr;
    ImageComponent* mWinFadeImage = nullptr;
    GameObject* mWinText = nullptr;
    ImageComponent* mWinTextImage = nullptr;
    GameObject* mWinLineRight = nullptr;
    Transform2DComponent* mWinLineRightTransfrom = nullptr;
    GameObject* mWinLineLeft = nullptr;
    Transform2DComponent* mWinLineLeftTransfrom = nullptr;
    
    // Video
    GameObject* mVideoGO = nullptr;
    VideoComponent* mVideoComponent = nullptr;
    bool mIsVideoPlaying = false;
    GameObject* mVideoBtnGO = nullptr;
    ButtonComponent* mVideoBtn = nullptr;
    int mVideoAudio = -1;

    //Highlights
    bool mGrenadeHL = false;
    TimerScript mGrenadeHLTimer;
    GameObject* mGrenadeHLGO = nullptr;
    GameObject* mGrenadeImageGO = nullptr;
    ImageComponent* mGrenadeImage = nullptr;

    bool mUltimateHL = false;
    TimerScript mUltimateHLTimer;
    GameObject* mUltimateHLGO = nullptr;
    ImageComponent* mUltimateBorder = nullptr;
    GameObject* mUltimateImageGO = nullptr;
    ImageComponent* mUltimateImage = nullptr;

    bool mGunHL = false;
    TimerScript mGunHLTimer;
    GameObject* mGunHLGO = nullptr;

    float mAlpha = 0.0f;

    //Hud
    GameObject* mHudGO = nullptr;

    // Enemies
    GameObject* mEnemyGO = nullptr;
    GameObject* mEnemy1GO = nullptr;
    GameObject* mEnemy2GO = nullptr;
    GameObject* mEnemy3GO = nullptr;
    GameObject* mEnemy4GO = nullptr;
    GameObject* mEnemy5GO = nullptr;


    // Buttons change
    GameObject* mControllerWeaponBinding = nullptr;
    GameObject* mControllerUltiBinding = nullptr;
    GameObject* mControllerGrenadeBinding = nullptr;
    GameObject* mControllerDialogueNextBinding = nullptr;
    GameObject* mControllerDialogueSkipBinding = nullptr;
    GameObject* mControllerVideoBinding = nullptr;
    GameObject* mControllerCollectibleOpenBinding = nullptr;
    GameObject* mControllerCollectibleCloseBinding = nullptr;
    GameObject* mControllerMenuSelectBinding = nullptr;
    GameObject* mControllerMenuBackBinding = nullptr;
    GameObject* mControllerCreditsSkipBinding = nullptr;
    GameObject* mControllerPresentationSkipBinding = nullptr;

    GameObject* mKeyboardWeaponBinding = nullptr;
    GameObject* mKeyboardUltiBinding = nullptr;
    GameObject* mKeyboardGrenadeBinding = nullptr;
    GameObject* mKeyboardDialogueNextBinding = nullptr;
    GameObject* mKeyboardDialogueSkipBinding = nullptr;
    GameObject* mKeyboardVideoBinding = nullptr;
    GameObject* mKeyboardCollectibleOpenBinding = nullptr;
    GameObject* mKeyboardCollectibleCloseBinding = nullptr;
    GameObject* mKeyboardMenuSelectBinding = nullptr;
    GameObject* mKeyboardMenuBackBinding = nullptr;
    GameObject* mKeyboardCreditsSkipBinding = nullptr;
    GameObject* mKeyboardPresentationSkipBinding = nullptr;
};