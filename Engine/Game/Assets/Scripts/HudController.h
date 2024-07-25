#pragma once
// Link to design
//https://www.figma.com/design/J6S4FzW8Yn38GOiMe7XdYm/GAME_UI?node-id=625-249&t=mL60BsxKQdYvFkMC-0

#include <Script.h>
#include "Macros.h"

class ButtonComponent;
class SliderComponent;
class TextComponent;
class ImageComponent;
class PlayerController;
class Sanity;
class Dialog;
enum class EnergyType;

enum class SCREEN {
    LOAD,
    LOSE,
    WIN,
    PAUSE
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
    void SwitchWeapon();
    void SetGrenadeCooldown(float cooldown);
    void SetUltimateCooldown(float cooldown);
    void SetScreen(SCREEN menu, bool active);

    SliderComponent* mHealthGradualSlider = nullptr;

    void SetSanity();
    void SetDialog();
private:
    HudController(GameObject* owner);
    ~HudController();

    void Loading();
    void Controls();
    void ButtonHover();
    void ButtonClick();
    bool Delay(float delay);

    void OnWinButtonClick();
    void OnTryAgainButtonClick();
    void OnTryAgainButtonHoverOn();
    void OnTryAgainButtonHoverOff();
    void OnLoseButtonClick();
    void OnLoseButtonHoverOn();
    void OnLoseButtonHoverOff();

    void OnContinueBtnClick();
    void OnContinueBtnHoverOn();
    void OnContinueBtnHoverOff();

    void OnOptionsBtnClick();
    void OnOptionsBtnHoverOn();
    void OnOptionsBtnHoverOff();

    void OnMainMenuBtnClick();
    void OnMainMenuBtnHoverOn();
    void OnMainMenuBtnHoverOff();


    // Pause Menu
    GameObject* mPauseScreen = nullptr;
    GameObject* mContinueBtnGO = nullptr;
    GameObject* mOptionsBtnGO = nullptr;
    GameObject* mMainMenuBtnGO = nullptr;
    ButtonComponent* mContinueBtn = nullptr;
    ButtonComponent* mOptionsBtn = nullptr;
    ButtonComponent* mMainMenuBtn = nullptr;
    GameObject* mOptionsPanel = nullptr;

    GameObject* mWinScreen = nullptr;
    GameObject* mLoseScreen = nullptr;
    GameObject* mLoadingScreen = nullptr;
    GameObject* mHealthGO = nullptr;
    GameObject* mHealthGradualGO = nullptr;
    GameObject* mAmmoGO = nullptr;
    GameObject* mWeaponMeleeGO = nullptr;
    GameObject* mSecondWeaponMeleeGO = nullptr;
    GameObject* mWeaponRangeGO = nullptr;
    GameObject* mSecondWeaponRangeGO = nullptr;
    GameObject* mGrenadeSliderGO = nullptr;
    GameObject* mUltimateSliderGO = nullptr;
    GameObject* mEnergyGO = nullptr;
    GameObject* mEnergyImageGO = nullptr;
    GameObject* mFeedbackGO = nullptr;

    GameObject* mTryAgainBtnGO = nullptr;
    GameObject* mLoseMenuBtnGO = nullptr;
    GameObject* mWinMenuBtnGO = nullptr;
    ButtonComponent* mLoseBtn = nullptr;
    ButtonComponent* mWinBtn = nullptr;
    ButtonComponent* mTryAgainBtn = nullptr;

    SliderComponent* mHealthSlider = nullptr;
    TextComponent* mAmmoText = nullptr;
    TextComponent* mEnergyText = nullptr;
    ImageComponent* mEnergyImage = nullptr;
    SliderComponent* mGrenadeSlider = nullptr;
    SliderComponent* mUltimateSlider = nullptr;
    ImageComponent* mFeedbackImage = nullptr;

    //Boss Health bar
    GameObject* mBossHealthGO = nullptr;
    GameObject* mBossHealthGradualGO = nullptr;
    float mBossHealth = 0.0;
    SliderComponent* mBossHealthSlider = nullptr;
    SliderComponent* mBossHealthGradualSlider = nullptr;

    float mTargetHealth = 1;

    //bool mLoading = false;
    bool mPaused = false;
    float mTimeScreen = 5.0f;
    float mTimePassed = 0.0f;

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
};