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
    MEMBER(MemberType::GAMEOBJECT, mWeaponGO);
    MEMBER(MemberType::GAMEOBJECT, mSecondWeaponGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mAmmoGO);
    SEPARATOR("Screens");
    MEMBER(MemberType::GAMEOBJECT, mPauseScreen);
    MEMBER(MemberType::GAMEOBJECT, mWinScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoseScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoadingScreen);
    SEPARATOR("Buttons");
    MEMBER(MemberType::GAMEOBJECT, mYesGO);
    MEMBER(MemberType::GAMEOBJECT, mNoGO);
    END_CREATE;
}

HudController::HudController(GameObject* owner) : Script(owner) {}

HudController::~HudController()
{
    
}

void HudController::Start()
{
    mPauseScreen->SetEnabled(false);
    mWinScreen->SetEnabled(false);
    mLoseScreen->SetEnabled(false);
    mLoadingScreen->SetEnabled(false);

    // Component cast
    mWinBtn = static_cast<ButtonComponent*>(mWinScreen->GetComponent(ComponentType::BUTTON));
    mLoseBtn = static_cast<ButtonComponent*>(mLoseScreen->GetComponent(ComponentType::BUTTON));
    mYesBtn = static_cast<ButtonComponent*>(mYesGO->GetComponent(ComponentType::BUTTON));
    mNoBtn = static_cast<ButtonComponent*>(mNoGO->GetComponent(ComponentType::BUTTON));

    mHealthSlider = static_cast<SliderComponent*>(mHealthGO->GetComponent(ComponentType::SLIDER));
    mHealthGradualSlider = static_cast<SliderComponent*>(mHealthGradualGO->GetComponent(ComponentType::SLIDER));
    mAmmoText = static_cast<TextComponent*>(mAmmoGO->GetComponent(ComponentType::TEXT));
    mGrenadeImage = static_cast<ImageComponent*>(mGrenadeGO->GetComponent(ComponentType::IMAGE));
    mGrenadeSlider = static_cast<SliderComponent*>(mGrenadeSliderGO->GetComponent(ComponentType::SLIDER));
    mWeaponImage = static_cast<ImageComponent*>(mWeaponGO->GetComponent(ComponentType::IMAGE));
    mSecondWeaponImage = static_cast<ImageComponent*>(mSecondWeaponGO->GetComponent(ComponentType::IMAGE));

    // Click events
    mWinBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnWinButtonClick, this)));
    mLoseBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnLoseButtonClick, this)));
    mYesBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnYesButtonClick, this)));
    mNoBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnNoButtonClick, this)));

    // Hovers events
    mYesBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&HudController::OnYesButtonHoverOn, this)));
    mNoBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&HudController::OnNoButtonHoverOn, this)));
    mYesBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&HudController::OnYesButtonHoverOff, this)));
    mNoBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&HudController::OnNoButtonHoverOff, this)));
}

void HudController::Update()
{
    Controls();
    Loading();

    // Gradually decrease the gradual health slider
    if (mHealthGradualSlider->GetValue() > mTargetHealth)
    {
        mHealthGradualSlider->SetValue(mHealthGradualSlider->GetValue() - 0.15f * App->GetDt());
    }
    else if (mHealthGradualSlider->GetValue() < mTargetHealth) {
        mHealthGradualSlider->SetValue(mTargetHealth);
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

void HudController::Controls()
{
    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
    {
        mPaused = !mPaused;
        mPauseScreen->SetEnabled(mPaused);
    }
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
    mAmmoText->SetText(std::to_string(ammo));
}

void HudController::SetHealth(float health)
{
    mHealthSlider->SetValue(health);
    mTargetHealth = health;
}

void HudController::SwitchWeapon()
{
    if (mWeaponGO->IsEnabled())
    {
        mWeaponGO->SetEnabled(false);
        mSecondWeaponGO->SetEnabled(true);
        return;
    }
    if (mSecondWeaponGO->IsEnabled())
    {
        mSecondWeaponGO->SetEnabled(false);
        mWeaponGO->SetEnabled(true);
        return;
    }
}

void HudController::SetGrenadeCooldown(float cooldown)
{
    mGrenadeSlider->SetValue(cooldown);
}

void HudController::SetScreen(SCREEN name, bool active)
{
    switch (name) {
        case SCREEN::LOAD:
            mLoadingScreen->SetEnabled(active);
            break;
        case SCREEN::LOSE:
            mLoseScreen->SetEnabled(active);
            break;
        case SCREEN::WIN:
            mWinScreen->SetEnabled(active);
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

void HudController::OnYesButtonClick()
{
    mLoading = true;
}

void HudController::OnNoButtonClick()
{
    mPaused = false;
    mPauseScreen->SetEnabled(false);
}

#pragma endregion

#pragma region Hover Events

void HudController::OnYesButtonHoverOn()
{
    ImageComponent* image = static_cast<ImageComponent*>(mYesGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(0.7f, 0.7f, 0.7f));
}

void HudController::OnNoButtonHoverOn()
{
    ImageComponent* image = static_cast<ImageComponent*>(mNoGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(0.7f, 0.7f, 0.7f));
}

void HudController::OnYesButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mYesGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(1, 1, 1));
}

void HudController::OnNoButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mNoGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(1, 1, 1));
}

#pragma endregion