#include "HudController.h"

#include "GameObject.h"
#include "Application.h"
#include "MainMenu.h"
#include "GameManager.h"
#include "ModuleInput.h"
#include "Keys.h"

#include "ButtonComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "SliderComponent.h"


CREATE(HudController)
{
    CLASS(owner);
    SEPARATOR("HUD");
    MEMBER(MemberType::GAMEOBJECT, mHealthGO);
    MEMBER(MemberType::GAMEOBJECT, mHealthGradualGO);
    MEMBER(MemberType::GAMEOBJECT, mWeaponMeleeGO);
    MEMBER(MemberType::GAMEOBJECT, mWeaponRangeGO);
    MEMBER(MemberType::GAMEOBJECT, mSecondWeaponMeleeGO);
    MEMBER(MemberType::GAMEOBJECT, mSecondWeaponRangeGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mAmmoGO);
    SEPARATOR("Pause Screen");
    MEMBER(MemberType::GAMEOBJECT, mPauseScreen);
    MEMBER(MemberType::GAMEOBJECT, mContinueBtnGO);
    MEMBER(MemberType::GAMEOBJECT, mOptionsBtnGO);
    MEMBER(MemberType::GAMEOBJECT, mMainMenuBtnGO);
    MEMBER(MemberType::GAMEOBJECT, mOptionsPanel);
    SEPARATOR("Screens");
    MEMBER(MemberType::GAMEOBJECT, mWinScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoseScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoadingScreen);
    END_CREATE;
}

HudController::HudController(GameObject* owner) : Script(owner) {}

HudController::~HudController()
{
    
}

void HudController::Start()
{
    if (mPauseScreen) {
        mPauseScreen->SetEnabled(false);
        if (mContinueBtnGO)
        {
            mContinueBtn = static_cast<ButtonComponent*>(mContinueBtnGO->GetComponent(ComponentType::BUTTON));
            mContinueBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnContinueBtnClick, this)));
            mContinueBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&HudController::OnContinueBtnHoverOn, this)));
            mContinueBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&HudController::OnContinueBtnHoverOff, this)));
        }
        if (mOptionsBtnGO)
        {
            mOptionsBtn = static_cast<ButtonComponent*>(mOptionsBtnGO->GetComponent(ComponentType::BUTTON));
            mOptionsBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnOptionsBtnClick, this)));
            mOptionsBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&HudController::OnOptionsBtnHoverOn, this)));
            mOptionsBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&HudController::OnOptionsBtnHoverOff, this)));
        }
        if (mMainMenuBtnGO)
        {
            mMainMenuBtn = static_cast<ButtonComponent*>(mMainMenuBtnGO->GetComponent(ComponentType::BUTTON));
            mMainMenuBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnMainMenuBtnClick, this)));
            mMainMenuBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&HudController::OnMainMenuBtnHoverOn, this)));
            mMainMenuBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&HudController::OnMainMenuBtnHoverOff, this)));
        }
        if (mOptionsPanel) mOptionsPanel->SetEnabled(false);
    }


    if (mWinScreen) 
    {
        mWinScreen->SetEnabled(false);
        mWinBtn = static_cast<ButtonComponent*>(mWinScreen->GetComponent(ComponentType::BUTTON));
        mWinBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnWinButtonClick, this)));
    }
    if (mLoseScreen)
    {
        mLoseScreen->SetEnabled(false);
        mLoseBtn = static_cast<ButtonComponent*>(mLoseScreen->GetComponent(ComponentType::BUTTON));
        mLoseBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnLoseButtonClick, this)));
    }
    if (mLoadingScreen) mLoadingScreen->SetEnabled(false);

    

    if (mHealthGO)
    {
        mHealthSlider = static_cast<SliderComponent*>(mHealthGO->GetComponent(ComponentType::SLIDER));
        mHealthSlider->SetValue(1.0f);
    }

    if (mHealthGradualGO) 
    {
        mHealthGradualSlider = static_cast<SliderComponent*>(mHealthGradualGO->GetComponent(ComponentType::SLIDER));
        mHealthGradualSlider->SetValue(1.0f);
    }

    if (mGrenadeSliderGO)
    {
        mGrenadeSlider = static_cast<SliderComponent*>(mGrenadeSliderGO->GetComponent(ComponentType::SLIDER));
        mGrenadeSlider->SetValue(0.001f);
    }

    if (mAmmoGO) mAmmoText = static_cast<TextComponent*>(mAmmoGO->GetComponent(ComponentType::TEXT));

}

void HudController::Update()
{
    Loading();

    if (GameManager::GetInstance()->IsPaused()) return;

    // Gradually decrease the gradual health slider
    if (mHealthGradualSlider != nullptr)
    {
        if (mHealthGradualSlider->GetValue() > mTargetHealth)
        {
            mHealthGradualSlider->SetValue(mHealthGradualSlider->GetValue() - 0.15f * App->GetDt());
        }
        else if (mHealthGradualSlider->GetValue() < mTargetHealth) 
        {
            mHealthGradualSlider->SetValue(mTargetHealth);
        }
    }

    // Grenade cooldown update
    if (mGrenadeSlider != nullptr) 
    {
        if (mGrenadeTimer <= mGrenadeCooldown) 
        {
            mGrenadeTimer += App->GetDt();
            mGrenadeSlider->SetValue(mGrenadeCooldown / mGrenadeTimer);
        }
    }
}

bool HudController::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}

void HudController::Loading()
{
    if (mLoading)
    {
        mLoadingScreen->SetEnabled(true);

        if (Delay(0.1f))
        {
            mLoading = false;
            GameManager::GetInstance()->LoadLevel("MainMenu.json");
        }
    }
}

void HudController::SetAmmo(int ammo)
{
    if (mAmmoText) mAmmoText->SetText(std::to_string(ammo));
}

void HudController::SetHealth(float health)
{
    if (mHealthSlider) mHealthSlider->SetValue(health);
    mTargetHealth = health;
}

void HudController::SwitchWeapon()
{
    if (!mWeaponMeleeGO || !mWeaponRangeGO || !mSecondWeaponMeleeGO || !mSecondWeaponRangeGO) return;

    if (mWeaponMeleeGO->IsEnabled())
    {
        mWeaponMeleeGO->SetEnabled(false);
        mWeaponRangeGO->SetEnabled(true);
        mSecondWeaponMeleeGO->SetEnabled(true);
        mSecondWeaponRangeGO->SetEnabled(false);
    }
    else
    {
        mWeaponMeleeGO->SetEnabled(true);
        mWeaponRangeGO->SetEnabled(false);
        mSecondWeaponMeleeGO->SetEnabled(false);
        mSecondWeaponRangeGO->SetEnabled(true);
    }
}

void HudController::SetGrenadeCooldown(float cooldown)
{
    mGrenadeCooldown = cooldown;
    mGrenadeTimer = 0.0f;
}

void HudController::SetScreen(SCREEN name, bool active)
{
    switch (name) {
        case SCREEN::LOAD:
            if (mLoadingScreen) mLoadingScreen->SetEnabled(active);
            break;
        case SCREEN::LOSE:
            if (mLoseScreen) mLoseScreen->SetEnabled(active);
            break;
        case SCREEN::WIN:
            if (mWinScreen) mWinScreen->SetEnabled(active);
            break;
        case SCREEN::PAUSE:
            if (mPauseScreen) mPauseScreen->SetEnabled(active);
            break;
        default:
            break;
    }
}

#pragma region Click Events

void HudController::OnWinButtonClick()
{
    mLoading = true;
}

void HudController::OnLoseButtonClick()
{
    mLoading = true;
}

#pragma endregion

#pragma region Btn Events

void HudController::OnContinueBtnClick()
{
    GameManager::GetInstance()->SetPaused(false);
}

void HudController::OnContinueBtnHoverOn()
{
    ImageComponent* image = static_cast<ImageComponent*>(mContinueBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.25f);
}

void HudController::OnContinueBtnHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mContinueBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

void HudController::OnOptionsBtnClick()
{
    mOptionsPanel->SetEnabled(true);
}

void HudController::OnOptionsBtnHoverOn()
{
    ImageComponent* image = static_cast<ImageComponent*>(mOptionsBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.25f);
}

void HudController::OnOptionsBtnHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mOptionsBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

void HudController::OnMainMenuBtnClick()
{
    GameManager::GetInstance()->LoadLevel("MainMenu");
}

void HudController::OnMainMenuBtnHoverOn()
{
    ImageComponent* image = static_cast<ImageComponent*>(mMainMenuBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.25f);
}

void HudController::OnMainMenuBtnHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mMainMenuBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

#pragma endregion