#include "HudController.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "PlayerStats.h"

#include "GameObject.h"
#include "ButtonComponent.h"
#include "ImageComponent.h"
#include "TextComponent.h"
#include "Transform2DComponent.h"
#include "SliderComponent.h"
#include "ScriptComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerController.h"
#include "MainMenu.h"
#include "Keys.h"
#include "Dialog.h"
#include "Sanity.h"




CREATE(HudController)
{
    CLASS(owner);
    SEPARATOR("HUD");
    MEMBER(MemberType::GAMEOBJECT, mHealthGO);
    MEMBER(MemberType::GAMEOBJECT, mHealthGradualGO);
    MEMBER(MemberType::GAMEOBJECT, mHealthIconGO);
    MEMBER(MemberType::GAMEOBJECT, mWeaponRangeGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mUltimateSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mAmmoGO);
    MEMBER(MemberType::GAMEOBJECT, mEnergyGO);
    MEMBER(MemberType::GAMEOBJECT, mEnergyImageGO);
    MEMBER(MemberType::GAMEOBJECT, mFeedbackGO);
    MEMBER(MemberType::GAMEOBJECT, mBossHealthGO);
    MEMBER(MemberType::GAMEOBJECT, mBossHealthGradualGO);
    SEPARATOR("Pause Screen");
    MEMBER(MemberType::GAMEOBJECT, mPauseScreen);
    MEMBER(MemberType::GAMEOBJECT, mFadeoutScreen);
    SEPARATOR("Screens");
    MEMBER(MemberType::GAMEOBJECT, mWinScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoseScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoadingScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoadingSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mTryAgainBtnGO);
    MEMBER(MemberType::GAMEOBJECT, mLoseMenuBtnGO);
    MEMBER(MemberType::GAMEOBJECT, mWinMenuBtnGO);
    
    SEPARATOR("Sanity & Dialog");
    MEMBER(MemberType::GAMEOBJECT, mSanityGO);
    MEMBER(MemberType::GAMEOBJECT, mDialogGO);

    SEPARATOR("Collectible");
    MEMBER(MemberType::GAMEOBJECT, mCollectibleScreen);
    MEMBER(MemberType::GAMEOBJECT, mCollectibleTextGO);
    MEMBER(MemberType::GAMEOBJECT, mCollectibleContinueBtnGO);
    MEMBER(MemberType::GAMEOBJECT, mInteractGO);

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
    if (mPauseScreen) mPauseScreen->SetEnabled(false);

    if (mWinScreen) 
    {
        mWinScreen->SetEnabled(false);
        if (mWinMenuBtnGO) mWinBtn = static_cast<ButtonComponent*>(mWinMenuBtnGO->GetComponent(ComponentType::BUTTON));
        if (mWinBtn) mWinBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnWinButtonClick, this)));
    }
    if (mLoseScreen)
    {
        mLoseScreen->SetEnabled(false);
        if (mLoseMenuBtnGO) mLoseBtn = static_cast<ButtonComponent*>(mLoseMenuBtnGO->GetComponent(ComponentType::BUTTON));
        if (mTryAgainBtnGO) mTryAgainBtn = static_cast<ButtonComponent*>(mTryAgainBtnGO->GetComponent(ComponentType::BUTTON));
        if (mLoseBtn) mLoseBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnLoseButtonClick, this)));
        if (mLoseBtn) mLoseBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&HudController::OnLoseButtonHoverOn, this)));
        if (mLoseBtn) mLoseBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&HudController::OnLoseButtonHoverOff, this)));
        if (mTryAgainBtn) mTryAgainBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnTryAgainButtonClick, this)));
        if (mTryAgainBtn) mTryAgainBtn->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&HudController::OnTryAgainButtonHoverOn, this)));
        if (mTryAgainBtn) mTryAgainBtn->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&HudController::OnTryAgainButtonHoverOff, this)));
    }
    if (mLoadingScreen) mLoadingScreen->SetEnabled(false);
    if (mLoadingSliderGO) 
    {
        mLoadingSlider = static_cast<SliderComponent*>(mLoadingSliderGO->GetComponent(ComponentType::SLIDER));
        mLoadingSlider->SetValue(0.01f);
    }
    

    if (mCollectibleScreen)
    {
        mCollectibleScreen->SetEnabled(false);
        if (mCollectibleTextGO) mLoreText = static_cast<TextComponent*>(mCollectibleTextGO->GetComponent(ComponentType::TEXT));
        if (mCollectibleContinueBtnGO) 
        {
            mCollectibleContinueBtn = static_cast<ButtonComponent*>(mCollectibleContinueBtnGO->GetComponent(ComponentType::BUTTON));
            mCollectibleContinueBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnCollectibleContinueBtnClick, this)));
        }
    }

    if (mHealthGO)
    {
        mHealthSlider = static_cast<SliderComponent*>(mHealthGO->GetComponent(ComponentType::SLIDER));
        mHealthImage = static_cast<ImageComponent*>(mHealthGO->GetChildren()[1]->GetComponent(ComponentType::IMAGE));
        mHealthSlider->SetValue(1.0f);
    }

    if (mHealthIconGO)
    {
        mHealthIcon = static_cast<ImageComponent*>(mHealthIconGO->GetComponent(ComponentType::IMAGE));
    }
    
    if (mHealthGradualGO) 
    {
        mHealthGradualSlider = static_cast<SliderComponent*>(mHealthGradualGO->GetComponent(ComponentType::SLIDER));
        mHealthGradualImage = static_cast<ImageComponent*>(mHealthGradualGO->GetChildren()[1]->GetComponent(ComponentType::IMAGE));
        mHealthGradualSlider->SetValue(1.0f);
    }

    SetMaxHealth(App->GetScene()->GetPlayerStats()->GetMaxHealth());
    
    if (mBossHealthGO)
    {
        mBossHealthSlider = static_cast<SliderComponent*>(mBossHealthGO->GetComponent(ComponentType::SLIDER));
        mBossHealthSlider->SetValue(1.0f);
    }

    if (mBossHealthGradualGO) 
    {
        mBossHealthGradualSlider = static_cast<SliderComponent*>(mBossHealthGradualGO->GetComponent(ComponentType::SLIDER));
        mBossHealthGradualSlider->SetValue(1.0f);
    }

    if (mGrenadeSliderGO)
    {
        mGrenadeSlider = static_cast<SliderComponent*>(mGrenadeSliderGO->GetComponent(ComponentType::SLIDER));
        mGrenadeSlider->SetValue(0.001f);
    }

    if (mUltimateSliderGO)
    {
        mUltimateSlider = static_cast<SliderComponent*>(mUltimateSliderGO->GetComponent(ComponentType::SLIDER));
        mUltimateSlider->SetValue(0.001f);
    }

    if (mAmmoGO) mAmmoText = static_cast<TextComponent*>(mAmmoGO->GetComponent(ComponentType::TEXT));
    if (mEnergyGO) mEnergyText = static_cast<TextComponent*>(mEnergyGO->GetComponent(ComponentType::TEXT));
    if (mEnergyImageGO) mEnergyImage = static_cast<ImageComponent*>(mEnergyImageGO->GetComponent(ComponentType::IMAGE));
    if (mFeedbackGO) mFeedbackImage = static_cast<ImageComponent*>(mFeedbackGO->GetComponent(ComponentType::IMAGE));
    if (mInteractGO) 
    {
        mInteractGO->SetEnabled(false);
        const std::vector<GameObject*> children = mInteractGO->GetChildren();
        mInteractText = static_cast<TextComponent*>(children[0]->GetComponent(ComponentType::TEXT));
    }

    if (mSanityGO) mSanity = static_cast<Sanity*>(static_cast<ScriptComponent*>(mSanityGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    if (mDialogGO) mDialog = static_cast<Dialog*>(static_cast<ScriptComponent*>(mDialogGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    if (mFadeoutScreen)
    {
        mFadeoutImage = static_cast<ImageComponent*>(mFadeoutScreen->GetComponent(ComponentType::IMAGE));
        mFadeoutScreen->SetEnabled(true);
    }

    SetDialog();
}

void HudController::Update()
{
    if (mFadeoutImage && mFadeIn) FadeIn();
    else if (mFadeoutImage && !mFadeIn) FadeOut();
    if(mLoadlevel == true && mLoadingSlider->GetValue() < 1) mLoadingSlider->SetValue(mLoadingSlider->GetValue() + 0.01);

    Controls();

    if (GameManager::GetInstance()->IsPaused()) return;

    // Gradually decrease the gradual health slider
    if (mHealthGradualSlider)
    {
        float currentGradualValue = mHealthGradualSlider->GetValue();

        // If health is decreasing
        if (currentGradualValue > mTargetHealth)
        {
            mHealthGradualImage->SetColor(float3(254.0f /255.0f, 224.0f /255.0f, 193.0f /255.0f));  // Yellow when decreasing
            mHealthGradualSlider->SetValue(currentGradualValue - 0.15f * App->GetDt());

            // Update health slider instantly to final value
            if (mHealthSlider) mHealthSlider->SetValue(mTargetHealth);
        }
        // If health is increasing
        else if (currentGradualValue < mTargetHealth)
        {
            mHealthGradualImage->SetColor(float3(231.0f /255.0f, 253.0f /255.0f, 242.0f /255.0f));  // Blue when increasing
            mHealthGradualSlider->SetValue(currentGradualValue + 0.15f * App->GetDt());

            // Only set health slider value when gradual slider reaches target
            if (currentGradualValue + 0.15f * App->GetDt() >= mTargetHealth)
            {
                if (mHealthSlider) mHealthSlider->SetValue(mTargetHealth);
            }
        }
    }
    if (mBossHealthGradualSlider && mBossHealthSlider)
    {
        float targetHealth = mBossHealthSlider->GetValue();
        if (mBossHealthGradualSlider->GetValue() > targetHealth)
        {
            mBossHealthGradualSlider->SetValue(mBossHealthGradualSlider->GetValue() - 0.15f * App->GetDt());
        }
        else if (mBossHealthGradualSlider->GetValue() < targetHealth)
        {
            mBossHealthGradualSlider->SetValue(targetHealth);
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

    // Ultimate cooldown update
    if (mUltimateSlider != nullptr && mUltimateCooldown != 0.0f)
    {
        if (mUltimateTimer <= mUltimateCooldown)
        {
            mUltimateTimer += App->GetDt();
            mUltimateSlider->SetValue(1 - (mUltimateTimer / mUltimateCooldown));
        }
        else
        {
            mUltimateCooldown = 0.0f;
        }
    }
}

void HudController::FadeIn()
{
    if (*mFadeoutImage->GetAlpha() >= 0.0f) mFadeoutImage->SetAlpha(*mFadeoutImage->GetAlpha() - 0.65f * App->GetDt());
}

void HudController::FadeOut()
{
    if (*mFadeoutImage->GetAlpha() <= 1.0f) mFadeoutImage->SetAlpha(*mFadeoutImage->GetAlpha() + 0.65f * App->GetDt());
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
    if (!GameManager::GetInstance()->IsPaused()) return;

    GameManager::GetInstance()->GetPlayerController()->SetIdleState();
}

void HudController::ButtonClick()
{
    if (mWinScreen->IsActive())
    {
        OnWinButtonClick();
        return;
    }
    if (mLoseScreen->IsActive())
    {
        OnLoseButtonClick();
        return;
    }
}

void HudController::SetSanity()
{
    mArenaCounter++;
    mSanityGO->SetEnabled(true);
    mSanity->CreateSelection(mArenaCounter);
}

void HudController::SetDialog()
{
    mDialogGO->SetEnabled(true);
    mDialog->StartDialog();
}

void HudController::DisableCollectible()
{
    OnCollectibleContinueBtnClick();
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
    if (health == 0) 
    {
        health = 0.001f;
        if (mFeedbackImage) mFeedbackImage->SetAlpha(1.0f);
    }
    else if (health < mHealthSlider->GetValue()) 
    {
        if (mFeedbackImage) mFeedbackImage->SetAlpha(1.0f);
    } 
    else
    {
        if (mFeedbackImage) mFeedbackImage->SetAlpha(0.0f);
    }

    if (mHealthSlider)
    {
        if (health < 0.3f)
        {
            mHealthImage->SetColor(float3(233.0f /255.0f, 112.0f /255.0f, 103.0f /255.0f));  // Red
            mHealthIcon->SetColor(float3(233.0f / 255.0f, 112.0f / 255.0f, 103.0f / 255.0f));
        }
        else
        {
            mHealthImage->SetColor(float3(59.0f/255.0f, 254.0f/255.0f, 223.0f /255.0f));  // Green
            mHealthIcon->SetColor(float3(59.0f / 255.0f, 254.0f / 255.0f, 223.0f / 255.0f));
        }
    }

    mTargetHealth = health;
}

void HudController::SetMaxHealth(float health)
{
    float newWidth = health * 2.8f;

    if (mHealthSlider)
    {
        float currentValue = mHealthSlider->GetValue();
        mHealthSlider->SetValue(1.0f);

        Transform2DComponent* transform = static_cast<Transform2DComponent*>(mHealthSlider->GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
        float2 currentSize = transform->GetSize();
        float3 currentPosition = transform->GetPosition();

        transform->SetSize(float2(newWidth, currentSize.y));

        float newPositionX = currentPosition.x + (newWidth - currentSize.x) / 2;
        transform->SetPosition(float3(newPositionX, currentPosition.y, 0));

        mHealthSlider->SetValue(currentValue);
    }
    if (mHealthGradualSlider)
    {
        float currentValue = mHealthGradualSlider->GetValue();
        mHealthGradualSlider->SetValue(1.0f);

        Transform2DComponent* transform = static_cast<Transform2DComponent*>(mHealthGradualSlider->GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
        float2 currentSize = transform->GetSize();
        float3 currentPosition = transform->GetPosition();

        transform->SetSize(float2(newWidth, currentSize.y));

        float newPositionX = currentPosition.x + (newWidth - currentSize.x) / 2;
        transform->SetPosition(float3(newPositionX, currentPosition.y, 0));

        mHealthGradualSlider->SetValue(currentValue);
    }
}


void HudController::SetGrenadeCooldown(float cooldown)
{
    mGrenadeCooldown = cooldown;
    mGrenadeTimer = 0.001f;
}

void HudController::SetUltimateCooldown(float cooldown)
{
    mUltimateCooldown = cooldown;
    mUltimateTimer = 0.001f;
}

void HudController::SetCollectibleText(std::string text)
{
    if (mLoreText) mLoreText->SetText(text);
}

void HudController::SetScreen(SCREEN name, bool active)
{
    switch (name) {
        case SCREEN::LOAD:
            if (mLoadingScreen) mLoadingScreen->SetEnabled(active);
            mLoadlevel = true;
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
        case SCREEN::COLLECTIBLE:
            if (mCollectibleScreen) mCollectibleScreen->SetEnabled(active);
            break;
        default:
            break;
    }
}

void HudController::SetInteract(bool active)
{
    if (mInteractGO)
    {
        if (mInteractText) 
        {
           if(GameManager::GetInstance()->UsingController()) mInteractText->SetText("Y to Interact");
           else mInteractText->SetText("F to Interact");
        }
        mInteractGO->SetEnabled(active);
    }
        
}

#pragma region Click Events

void HudController::OnWinButtonClick()
{
    GameManager::GetInstance()->LoadLevel("Assets/Scenes/MainMenu");
}

void HudController::OnLoseButtonClick()
{
    GameManager::GetInstance()->LoadLevel("Assets/Scenes/MainMenu");
}

void HudController::OnTryAgainButtonClick()
{
    GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level1Scene");
}

void HudController::OnTryAgainButtonHoverOn()
{
    ImageComponent* image = static_cast<ImageComponent*>(mTryAgainBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.25f);
}

void HudController::OnTryAgainButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mTryAgainBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

void HudController::OnLoseButtonHoverOn() 
{
    ImageComponent* image = static_cast<ImageComponent*>(mLoseMenuBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.25f);
}

void HudController::OnLoseButtonHoverOff() 
{
    ImageComponent* image = static_cast<ImageComponent*>(mLoseMenuBtnGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

#pragma endregion

#pragma region Btn Events

void HudController::OnCollectibleContinueBtnClick()
{
    if (mCollectibleScreen->IsActive()) {
        GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::MAINMENU_OK, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());

        SetScreen(SCREEN::COLLECTIBLE, false);
        GameManager::GetInstance()->SetPaused(false, false);
    }
}

void HudController::OnCollectibleContinueBtnHoverOn()
{
}

void HudController::OnCollectibleContinueBtnHoverOff()
{
}

void HudController::SetBossHealthBarEnabled(bool enabled)
{
    if (mBossHealthGO) mBossHealthGO->GetParent()->SetEnabled(enabled);

}

void HudController::SetBossHealth(float health)
{
    if (mBossHealthSlider)
    {
        mBossHealthSlider->SetValue(health);
    }
}

#pragma endregion