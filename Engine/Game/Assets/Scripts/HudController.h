#pragma once

//https://www.figma.com/design/J6S4FzW8Yn38GOiMe7XdYm/GAME_UI?node-id=625-249&t=mL60BsxKQdYvFkMC-0

#include <Script.h>
#include "Macros.h"

class ButtonComponent;
class SliderComponent;
class TextComponent;
class ImageComponent;

enum SCREEN {
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
    void SetHealth(float health);
    void SwitchWeapon();
    void SetGrenadeCooldown(float cooldown);
    void SetScreen(SCREEN menu, bool active);

    SliderComponent* mHealthGradualSlider = nullptr;
private:
    HudController(GameObject* owner);
    ~HudController();

    void Loading();
    void Controls();
    bool Delay(float delay);

    void OnWinButtonClick();
    void OnLoseButtonClick();

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

    ButtonComponent* mLoseBtn = nullptr;
    ButtonComponent* mWinBtn = nullptr;
    SliderComponent* mHealthSlider = nullptr;
    TextComponent* mAmmoText = nullptr;
    SliderComponent* mGrenadeSlider = nullptr;

    float mTargetHealth = 1;

    bool mLoading = false;
    bool mPaused = false;
    float mTimeScreen = 5.0f;
    float mTimePassed = 0.0f;

    GameObject* mMenuScript = nullptr;

    float mGrenadeCooldown = 0.0f;
    float mGrenadeTimer = 0.0f;
};