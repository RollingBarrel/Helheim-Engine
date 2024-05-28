#pragma once
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
    void OnYesButtonClick();
    void OnNoButtonClick();

    void OnYesButtonHoverOn();
    void OnNoButtonHoverOn();
    void OnYesButtonHoverOff();
    void OnNoButtonHoverOff();

    GameObject* mPauseScreen = nullptr;
    GameObject* mWinScreen = nullptr;
    GameObject* mLoseScreen = nullptr;
    GameObject* mLoadingScreen = nullptr;
    GameObject* mYesGO = nullptr;
    GameObject* mNoGO = nullptr;
    GameObject* mHealthGO = nullptr;
    GameObject* mHealthGradualGO = nullptr;
    GameObject* mAmmoGO = nullptr;
    GameObject* mWeaponMeleeGO = nullptr;
    GameObject* mSecondWeaponMeleeGO = nullptr;
    GameObject* mWeaponRangeGO = nullptr;
    GameObject* mSecondWeaponRangeGO = nullptr;
    GameObject* mGrenadeGO = nullptr;
    GameObject* mGrenadeSliderGO = nullptr;

    ButtonComponent* mLoseBtn = nullptr;
    ButtonComponent* mWinBtn = nullptr;
    ButtonComponent* mYesBtn = nullptr;
    ButtonComponent* mNoBtn = nullptr;
    SliderComponent* mHealthSlider = nullptr;
    TextComponent* mAmmoText = nullptr;
    SliderComponent* mGrenadeSlider = nullptr;

    float mTargetHealth = 1;

    bool mLoading = false;
    bool mPaused = false;
    float mTimeScreen = 5.0f;
    float mTimePassed = 0.0f;

    GameObject* mMenuScript = nullptr;
};