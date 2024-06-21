#include "HudController.h"

#include "GameObject.h"
#include "Application.h"
#include "MainMenu.h"
#include "GameManager.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"

#include "ButtonComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "Transform2DComponent.h"
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
    MEMBER(MemberType::GAMEOBJECT, mGrenadeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mAmmoGO);
    MEMBER(MemberType::GAMEOBJECT, mEnergyGO);
    MEMBER(MemberType::GAMEOBJECT, mEnergyImageGO);
    MEMBER(MemberType::GAMEOBJECT, mFeedbackGO);
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

HudController::HudController(GameObject* owner) : Script(owner) 
{
}

HudController::~HudController()
{
}

void HudController::Start()
{
    if (mPauseScreen) 
    {
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
    if (mEnergyGO) mEnergyText = static_cast<TextComponent*>(mEnergyGO->GetComponent(ComponentType::TEXT));
    if (mEnergyImageGO) mEnergyImage = static_cast<ImageComponent*>(mEnergyImageGO->GetComponent(ComponentType::IMAGE));
    if (mFeedbackGO) mFeedbackImage = static_cast<ImageComponent*>(mFeedbackGO->GetComponent(ComponentType::IMAGE));
}

void HudController::Update()
{
    Loading();

    if (GameManager::GetInstance()->IsPaused()) return;

    // Gradually decrease the gradual health slider
    if (mHealthGradualSlider)
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

    // Decrease the damage feedback
    if (mFeedbackImage && *(mFeedbackImage->GetAlpha()) >= 0.0f) {
        mFeedbackImage->SetAlpha(*(mFeedbackImage->GetAlpha()) - 0.4f * App->GetDt());
    }

    // Grenade cooldown update
    if (mGrenadeSlider != nullptr && mGrenadeCooldown != 0.0f) 
    {
        if (mGrenadeTimer <= mGrenadeCooldown) 
        {
            mGrenadeTimer += App->GetDt();
            mGrenadeSlider->SetValue(1 - (mGrenadeTimer / mGrenadeCooldown));
        }
        else
        {
            mGrenadeCooldown = 0.0f;
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

void HudController::SetEnergy(int energy, EnergyType type)
{
    if (mEnergyText) mEnergyText->SetText(std::to_string(energy));

    float3 color;
    switch (type)
    {
    case EnergyType::NONE:
        color = float3(100.0f,100.0f,100.0f);
        break;
    case EnergyType::BLUE:
        color = float3(0.0f, 0.0f, 200.0f);
        break;
    case EnergyType::RED:
        color = float3(200.0f, 0.0f, 0.0f);
        break;
    default:
        color = float3(100.0f, 100.0f, 100.0f);
        break;
    }

    if (mEnergyImage) mEnergyImage->SetColor(color);
    if (mEnergyText) mEnergyText->SetTextColor(color);

}

void HudController::SetHealth(float health)
{
    if (health < mHealthSlider->GetValue()) mFeedbackImage->SetAlpha(1.0f);
    if (mHealthSlider) mHealthSlider->SetValue(health);
    mTargetHealth = health;
}

void HudController::SetMaxHealth(float health)
{
    float newWidth = health * 3;

    if (mHealthSlider)
    {
        Transform2DComponent* transform = static_cast<Transform2DComponent*>(mHealthSlider->GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
        float2 currentSize = transform->GetSize();
        float3 currentPosition = transform->GetPosition();

        transform->SetSize(float2(newWidth, currentSize.y));

        float newPositionX = currentPosition.x + (newWidth - currentSize.x) / 2;
        transform->SetPosition(float3(newPositionX, currentPosition.y, 0));
    }
    if (mHealthGradualSlider)
    {
        Transform2DComponent* transform = static_cast<Transform2DComponent*>(mHealthGradualSlider->GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
        float2 currentSize = transform->GetSize();
        float3 currentPosition = transform->GetPosition();

        transform->SetSize(float2(newWidth, currentSize.y));

        float newPositionX = currentPosition.x + (newWidth - currentSize.x) / 2;
        transform->SetPosition(float3(newPositionX, currentPosition.y, 0));
    }
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
    mGrenadeTimer = 0.001f;
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