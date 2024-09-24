#pragma once
// Link to design
//https://www.figma.com/design/J6S4FzW8Yn38GOiMe7XdYm/GAME_UI?node-id=625-249&t=mL60BsxKQdYvFkMC-0

#include <Script.h>
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
enum class EnergyType;

enum class SCREEN {
    LOAD,
    LOSE,
    WIN,
    PAUSE,
    COLLECTIBLE
};

GENERATE_BODY(HudController);
class HudController : public Script
{
    FRIEND(HudController)
public:
    void Start();
    void Update();

    void SetAmmo(int ammo);
    void SetEnergy(int energy, EnergyType type);
    void SetHealth(float health);
    void SetBossHealth(float health);
    void SetBossHealthBarEnabled(bool enabled);
    void SetMaxHealth(float health);
    void SetGrenadeCooldown(float cooldown);
    void SetUltimateCooldown(float cooldown);
    void SetCollectibleText(std::string text);
    void SetScreen(SCREEN menu, bool active);
    void SetInteract(bool active);
    void SetPickupPrompt(bool active);

    void SetSanity();
    void SetDialog();
    void DisableCollectible();
    void SetFadein(bool value) { mFadeIn = value; }


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
    GameObject* mEnergyGO = nullptr;
    GameObject* mEnergyImageGO = nullptr;
    GameObject* mFeedbackGO = nullptr;
    GameObject* mCollectibleScreen = nullptr;
    GameObject* mCollectibleTextGO = nullptr;
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
    TextComponent* mEnergyText = nullptr;
    ImageComponent* mEnergyImage = nullptr;
    SliderComponent* mGrenadeSlider = nullptr;
    SliderComponent* mUltimateSlider = nullptr;
    ImageComponent* mFeedbackImage = nullptr;
    TextComponent* mLoreText = nullptr;
    TextComponent* mInteractText = nullptr;
    ButtonComponent* mCollectibleContinueBtn = nullptr;

    //Boss Health bar
    GameObject* mBossHealthGO = nullptr;
    GameObject* mBossHealthGradualGO = nullptr;
    float mBossHealth = 0.0;
    SliderComponent* mBossHealthSlider = nullptr;
    SliderComponent* mBossHealthGradualSlider = nullptr;

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

    int mCurrentOption = 0;

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
    bool mUltimateHL = false;
    TimerScript mUltimateHLTimer;
    GameObject* mUltimateHLGO = nullptr;
};