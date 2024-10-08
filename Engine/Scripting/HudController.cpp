#include "HudController.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleScene.h"

#include "PlayerStats.h"

#include "GameObject.h"
#include "ButtonComponent.h"
#include "ImageComponent.h"
#include "VideoComponent.h"
#include "TextComponent.h"
#include "Transform2DComponent.h"
#include "SliderComponent.h"
#include "ScriptComponent.h"

#include "GameManager.h"
#include "AudioManager.h"
#include "PlayerController.h"
#include "Keys.h"
#include "Dialog.h"
#include "Sanity.h"
#include "PauseMenu.h"
#include "Weapon.h"


CREATE(HudController)
{
    CLASS(owner);
    SEPARATOR("HUD");
    MEMBER(MemberType::GAMEOBJECT, mHudGO);
    MEMBER(MemberType::GAMEOBJECT, mHealthGO);
    MEMBER(MemberType::GAMEOBJECT, mHealthGradualGO);
    MEMBER(MemberType::GAMEOBJECT, mHealthIconGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeHLGO);
    MEMBER(MemberType::GAMEOBJECT, mGrenadeImageGO);
    MEMBER(MemberType::GAMEOBJECT, mUltimateSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mUltimateHLGO);
    MEMBER(MemberType::GAMEOBJECT, mUltimateImageGO);
    MEMBER(MemberType::GAMEOBJECT, mGunHLGO);
    MEMBER(MemberType::GAMEOBJECT, mAmmoGO);
    MEMBER(MemberType::GAMEOBJECT, mAmmoBaseGO);
    MEMBER(MemberType::GAMEOBJECT, mAmmoShotgunGO);
    MEMBER(MemberType::GAMEOBJECT, mAmmoSubGO);
    MEMBER(MemberType::GAMEOBJECT, mFeedbackGO);
    SEPARATOR("Boss health");
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
    
    SEPARATOR("Debug");
    MEMBER(MemberType::GAMEOBJECT, mDebugGO);
    MEMBER(MemberType::GAMEOBJECT, mGodmodeGO);
    MEMBER(MemberType::GAMEOBJECT, mInstakillGO);
    MEMBER(MemberType::GAMEOBJECT, mDialogueGO);

    SEPARATOR("Sanity & Dialog");
    MEMBER(MemberType::GAMEOBJECT, mSanityGO);
    MEMBER(MemberType::GAMEOBJECT, mDialogGO);

    SEPARATOR("Collectible");
    MEMBER(MemberType::GAMEOBJECT, mCollectibleScreen);
    MEMBER(MemberType::GAMEOBJECT, mCollectibleTextGO);
    MEMBER(MemberType::GAMEOBJECT, mCollectibleTitleGO);
    MEMBER(MemberType::GAMEOBJECT, mCollectibleSubtitleGO);
    MEMBER(MemberType::GAMEOBJECT, mCollectibleContinueBtnGO);
    MEMBER(MemberType::GAMEOBJECT, mInteractGO); 

    SEPARATOR("Weapon Pickups");
    MEMBER(MemberType::GAMEOBJECT, mPickupControllerGO);
    MEMBER(MemberType::GAMEOBJECT, mPickupKeyboardGO);

    SEPARATOR("Lose Animations");
    MEMBER(MemberType::GAMEOBJECT, mLoseFade);
    MEMBER(MemberType::GAMEOBJECT, mLoseBackText);
    MEMBER(MemberType::GAMEOBJECT, mLoseText);
    MEMBER(MemberType::GAMEOBJECT, mLoseLineRight);
    MEMBER(MemberType::GAMEOBJECT, mLoseLineLeft); 
    MEMBER(MemberType::GAMEOBJECT, mLoseMenuBtnClicked);
    MEMBER(MemberType::GAMEOBJECT, mTryAgainBtnClicked);

    SEPARATOR("Win Animations");
    MEMBER(MemberType::GAMEOBJECT, mWinFade);
    MEMBER(MemberType::GAMEOBJECT, mWinBackText);
    MEMBER(MemberType::GAMEOBJECT, mWinText);
    MEMBER(MemberType::GAMEOBJECT, mWinLineRight);
    MEMBER(MemberType::GAMEOBJECT, mWinLineLeft);
    
    SEPARATOR("Video");
    MEMBER(MemberType::GAMEOBJECT, mVideoGO);
    MEMBER(MemberType::GAMEOBJECT, mVideoBtnGO);

    SEPARATOR("Enemies");
    MEMBER(MemberType::GAMEOBJECT, mEnemyGO);
    MEMBER(MemberType::GAMEOBJECT, mEnemy1GO);
    MEMBER(MemberType::GAMEOBJECT, mEnemy2GO);
    MEMBER(MemberType::GAMEOBJECT, mEnemy3GO);
    MEMBER(MemberType::GAMEOBJECT, mEnemy4GO);
    MEMBER(MemberType::GAMEOBJECT, mEnemy5GO);

    SEPARATOR("BINDINGS");
    MEMBER(MemberType::GAMEOBJECT, mControllerWeaponBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerUltiBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerGrenadeBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerDialogueNextBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerDialogueSkipBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerVideoBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerCollectibleOpenBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerCollectibleCloseBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerMenuSelectBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerMenuBackBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerCreditsSkipBinding);
    MEMBER(MemberType::GAMEOBJECT, mControllerPresentationSkipBinding);

    MEMBER(MemberType::GAMEOBJECT, mKeyboardWeaponBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardUltiBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardGrenadeBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardDialogueNextBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardDialogueSkipBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardVideoBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardCollectibleOpenBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardCollectibleCloseBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardMenuSelectBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardMenuBackBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardCreditsSkipBinding);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardPresentationSkipBinding);

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
        mPauseMenu = static_cast<PauseMenu*>(static_cast<ScriptComponent*>(mPauseScreen->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
    }

    if (mWinScreen) 
    {
        mWinScreen->SetEnabled(false);
        if (mWinMenuBtnGO) mWinBtn = static_cast<ButtonComponent*>(mWinMenuBtnGO->GetComponent(ComponentType::BUTTON));
        if (mWinBtn) mWinBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnWinButtonClick, this)));

        mWinBackImage = static_cast<ImageComponent*>(mWinBackText->GetComponent(ComponentType::IMAGE));
        mWinFadeImage = static_cast<ImageComponent*>(mWinFade->GetComponent(ComponentType::IMAGE));
        mWinTextImage = static_cast<ImageComponent*>(mWinText->GetComponent(ComponentType::IMAGE));
        mWinLineRightTransfrom = static_cast<Transform2DComponent*>(mWinLineRight->GetComponent(ComponentType::TRANSFORM2D));
        mWinLineLeftTransfrom = static_cast<Transform2DComponent*>(mWinLineLeft->GetComponent(ComponentType::TRANSFORM2D));
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
    
        mLoseBackImage = static_cast<ImageComponent*>(mLoseBackText->GetComponent(ComponentType::IMAGE));
        mLoseFadeImage = static_cast<ImageComponent*>(mLoseFade->GetComponent(ComponentType::IMAGE));
        mLoseTextImage = static_cast<ImageComponent*>(mLoseText->GetComponent(ComponentType::IMAGE));
        mLoseLineRightTransfrom = static_cast<Transform2DComponent*>(mLoseLineRight->GetComponent(ComponentType::TRANSFORM2D));
        mLoseLineLeftTransfrom = static_cast<Transform2DComponent*>(mLoseLineLeft->GetComponent(ComponentType::TRANSFORM2D));
    
    }
    if (mLoadingScreen) mLoadingScreen->SetEnabled(false);
    if (mLoadingSliderGO) 
    {
        mLoadingSlider = static_cast<SliderComponent*>(mLoadingSliderGO->GetComponent(ComponentType::SLIDER));
        mLoadingSlider->SetValue(0.01f);
    }
    
    if (mDebugGO) 
    {
        mGodmodeImage = static_cast<ImageComponent*>(mGodmodeGO->GetComponent(ComponentType::IMAGE));
        mInstakillImage = static_cast<ImageComponent*>(mInstakillGO->GetComponent(ComponentType::IMAGE));
        mDialogueImage = static_cast<ImageComponent*>(mDialogueGO->GetComponent(ComponentType::IMAGE));
    }

    if (mCollectibleScreen)
    {
        mCollectibleScreen->SetEnabled(false);
        if (mCollectibleTextGO) mLoreText = static_cast<TextComponent*>(mCollectibleTextGO->GetComponent(ComponentType::TEXT));
        if (mCollectibleTitleGO) mTitleText = static_cast<TextComponent*>(mCollectibleTitleGO->GetComponent(ComponentType::TEXT));
        if (mCollectibleSubtitleGO) mSubtitleText = static_cast<TextComponent*>(mCollectibleSubtitleGO->GetComponent(ComponentType::TEXT));
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
        mBossHealthImage = static_cast<ImageComponent*>(mBossHealthGO->GetChildren()[1]->GetComponent(ComponentType::IMAGE));
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
        mGrenadeImage = static_cast<ImageComponent*>(mGrenadeImageGO->GetComponent(ComponentType::IMAGE));
    }

    if (mUltimateSliderGO)
    {
        mUltimateSlider = static_cast<SliderComponent*>(mUltimateSliderGO->GetComponent(ComponentType::SLIDER));
        mUltimateSlider->SetValue(0.001f);
        mUltimateBorder = static_cast<ImageComponent*>(mUltimateHLGO->GetComponent(ComponentType::IMAGE));
        mUltimateImage = static_cast<ImageComponent*>(mUltimateImageGO->GetComponent(ComponentType::IMAGE));
    }

    if (mAmmoGO) mAmmoText = static_cast<TextComponent*>(mAmmoGO->GetComponent(ComponentType::TEXT));

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

    if (mVideoGO)
    {
        mVideoComponent = static_cast<VideoComponent*>(mVideoGO->GetComponent(ComponentType::VIDEO));
        mVideoGO->SetEnabled(true);
        mVideoBtn = static_cast<ButtonComponent*>(mVideoBtnGO->GetComponent(ComponentType::BUTTON));
        mVideoBtn->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&HudController::OnVideoBackClick, this)));
    }
    if (mVideoComponent)
    {
        mVideoComponent->Play();
        PlayVideoAssociatedAudio();
        GameManager::GetInstance()->SetPaused(true, false);
        GameManager::GetInstance()->PauseBackgroundAudio(true);
        mIsVideoPlaying = true;
    }
    else if (App->GetScene()->GetName() != "Level3Scene")
    {
        SetDialog();
    }
}

void HudController::Update()
{
    if (mIsVideoPlaying && mVideoComponent)
    {
        bool stopVideo = false;
        if (!mVideoComponent->IsPlaying()) stopVideo = true;
        else if(App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN ||
            App->GetInput()->GetKey(Keys::Keys_BACKSPACE) == KeyState::KEY_DOWN ||
            App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_DOWN) 
            stopVideo = true;

        if (stopVideo)
        {
            OnVideoBackClick();
        }
    }

    if (mFadeoutImage && mFadeIn && !mIsVideoPlaying) FadeIn();
    else if (mFadeoutImage && !mFadeIn && !mIsVideoPlaying) FadeOut();
    if(mLoadlevel == true && mLoadingSlider->GetValue() < 1) mLoadingSlider->SetValue(mLoadingSlider->GetValue() + 0.01);

    Controls();

    if (mLoseFlag == true) LoseUpdate();
    if (mWinFlag == true) WinUpdate();

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
    if (mGrenadeHL && mGrenadeHLTimer.DelayWithoutReset(0.25f))
    {
        mGrenadeHLGO->SetEnabled(false);
        mGrenadeHL = false;
    }
    if (mGrenadeSlider != nullptr && mGrenadeCooldown != 0.0f) 
    {
        if (mGrenadeTimer <= mGrenadeCooldown) 
        {
            mGrenadeTimer += App->GetDt();
            mGrenadeSlider->SetValue(1 - (mGrenadeTimer / mGrenadeCooldown));
        }
        else
        {
            mGrenadeHL = true;
            mGrenadeHLGO->SetEnabled(true); 
            mGrenadeImage->SetAlpha(1.0f);
            mGrenadeHLTimer.Reset();
            mGrenadeCooldown = 0.0f;
        }
    }

    // Apply pulsating effect
    if (mUltimateHL)
    {
        mAlpha += App->GetDt() * 5.0f;
        mUltimateBorder->SetAlpha(abs(sinf(mAlpha)));
    }

    // Weapon highlight
    if (mGunHL && mGunHLTimer.DelayWithoutReset(0.25))
    {
        mGunHL = false;
        mGunHLGO->SetEnabled(false);
    }
}

void HudController::LoseUpdate()
{
    if (*mLoseFadeImage->GetAlpha() < 1.0f) mLoseFadeImage->SetAlpha(*mLoseFadeImage->GetAlpha() + 0.30f * App->GetDt());

    if (mLoseAnimationTimer.DelayWithoutReset(0.5f))
    {
        if (*mLoseBackImage->GetAlpha() < 1.0f) mLoseBackImage->SetAlpha(*mLoseBackImage->GetAlpha() + 0.30f * App->GetDt());
    }
    if (mLoseAnimationTimer.DelayWithoutReset(1.0f))
    {
        if (*mLoseTextImage->GetAlpha() < 0.8f) mLoseTextImage->SetAlpha(*mLoseTextImage->GetAlpha() + 0.30f * App->GetDt());
    }
    if (mLoseAnimationTimer.DelayWithoutReset(1.5f))
    {
        if (mLoseLineRightTransfrom->GetPosition().x > 300)
            mLoseLineRightTransfrom->SetPosition(mLoseLineRightTransfrom->GetPosition() - float3(400.0f, 0.0f, 0.0f) * App->GetDt());
        if (mLoseLineLeftTransfrom->GetPosition().x < -300)
            mLoseLineLeftTransfrom->SetPosition(mLoseLineLeftTransfrom->GetPosition() + float3(400.0f, 0.0f, 0.0f) * App->GetDt());
    }
}

void HudController::WinUpdate()
{
    if (*mWinFadeImage->GetAlpha() < 1.0f) mWinFadeImage->SetAlpha(*mWinFadeImage->GetAlpha() + 0.30f * App->GetDt());

    if (mWinAnimationTimer.DelayWithoutReset(0.5f))
    {
        if (*mWinBackImage->GetAlpha() < 1.0f) mWinBackImage->SetAlpha(*mWinBackImage->GetAlpha() + 0.30f * App->GetDt());
    }
    if (mWinAnimationTimer.DelayWithoutReset(1.0f))
    {
        if (*mWinTextImage->GetAlpha() < 0.8f) mWinTextImage->SetAlpha(*mWinTextImage->GetAlpha() + 0.30f * App->GetDt());
    }
    if (mWinAnimationTimer.DelayWithoutReset(1.5f))
    {
        if (mWinLineRightTransfrom->GetPosition().x > 300)
            mWinLineRightTransfrom->SetPosition(mWinLineRightTransfrom->GetPosition() - float3(400.0f, 0.0f, 0.0f) * App->GetDt());
        if (mWinLineLeftTransfrom->GetPosition().x < -300)
            mWinLineLeftTransfrom->SetPosition(mWinLineLeftTransfrom->GetPosition() + float3(400.0f, 0.0f, 0.0f) * App->GetDt());
    }
}

void HudController::FadeIn()
{
    if (*mFadeoutImage->GetAlpha() >= 0.0f) mFadeoutImage->SetAlpha(*mFadeoutImage->GetAlpha() - 0.65f * App->GetDt());
    if (*mFadeoutImage->GetAlpha() < 0.0f) mFadeoutImage->SetAlpha(0.0f);
}

void HudController::FadeOut()
{
    if (*mFadeoutImage->GetAlpha() <= 1.0f) mFadeoutImage->SetAlpha(*mFadeoutImage->GetAlpha() + 0.65f * App->GetDt());
    if (*mFadeoutImage->GetAlpha() > 1.0f) mFadeoutImage->SetAlpha(1.0f);
}

void HudController::LoseAnimation()
{
    mLoseFlag = true;
    mLoseAnimationTimer.Reset();

    mLoseBackImage->SetAlpha(0.0f);
    mLoseFadeImage->SetAlpha(0.0f);
    mLoseTextImage->SetAlpha(0.0f);

    mLoseLineRightTransfrom->SetPosition(float3(950.0f, 0.0f, 0.0f));
    mLoseLineLeftTransfrom->SetPosition(float3(-950.0f, 0.0f, 0.0f));
}

void HudController::WinAnimation()
{
    mWinFlag = true;
    mWinAnimationTimer.Reset();

    mWinBackImage->SetAlpha(0.0f);
    mWinFadeImage->SetAlpha(0.0f);
    mWinTextImage->SetAlpha(0.0f);

    mWinLineRightTransfrom->SetPosition(float3(950.0f, 0.0f, 0.0f));
    mWinLineLeftTransfrom->SetPosition(float3(-950.0f, 0.0f, 0.0f));
}

void HudController::PlayVideoAssociatedAudio()
{
    const char* videoName = mVideoComponent->GetName();
    bool isVideoLoop = mVideoComponent->GetLoop();

    if (strcmp(videoName, "Chrysalis_intro.mp4") == 0)
    {
        mVideoAudio = GameManager::GetInstance()->GetAudio()->Play(BGM::INTRO_VIDEO);
        GameManager::GetInstance()->GetAudio()->SetLoop(BGM::INTRO_VIDEO, mVideoAudio, isVideoLoop);
    }
}

void HudController::ReleaseVideoAssociatedAudio()
{
    const char* videoName = mVideoComponent->GetName();

    if (strcmp(videoName, "Chrysalis_intro.mp4") == 0)
    {
        mVideoAudio = GameManager::GetInstance()->GetAudio()->Release(BGM::INTRO_VIDEO, mVideoAudio);
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
    if (!GameManager::GetInstance()->IsPaused()) return;

    GameManager::GetInstance()->GetPlayerController()->SetIdleState();
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

void HudController::SetEnemyScreen(bool value, int enemy)
{
    mEnemyGO->SetEnabled(value);
    
    if (mEnemy1GO) mEnemy1GO->SetEnabled(false);
    if (mEnemy2GO) mEnemy2GO->SetEnabled(false);
    if (mEnemy3GO) mEnemy3GO->SetEnabled(false);
    if (mEnemy4GO) mEnemy4GO->SetEnabled(false);
    if (mEnemy5GO) mEnemy5GO->SetEnabled(false);

    switch(enemy)
    {
    case 1:
        if (mEnemy1GO)  mEnemy1GO->SetEnabled(true);
        break;
    case 2:
        if (mEnemy2GO) mEnemy2GO->SetEnabled(true);
        break;
    case 3:
        if (mEnemy3GO) mEnemy3GO->SetEnabled(true);
        break;
    case 4:
        if (mEnemy4GO) mEnemy4GO->SetEnabled(true);
        break;
    case 5:
        if (mEnemy5GO) mEnemy5GO->SetEnabled(true);
        break;
    default:
        break;
    }
}

void HudController::SetGodmode(bool value)
{
    value ? mGodmodeImage->SetColor(float3(1.0f, 1.0f, 1.0f)) : mGodmodeImage->SetColor(float3(0.0f, 0.0f, 0.0f));
    if (mInstakillImage->GetColor()->Equals(float3(0.0f, 0.0f, 0.0f)) &&
        mDialogueImage->GetColor()->Equals(float3(0.0f, 0.0f, 0.0f))) SetDebug(value);
}

void HudController::SetInstaKill(bool value)
{
    value ? mInstakillImage->SetColor(float3(1.0f, 1.0f, 1.0f)) : mInstakillImage->SetColor(float3(0.0f, 0.0f, 0.0f));
    if (mGodmodeImage->GetColor()->Equals(float3(0.0f, 0.0f, 0.0f)) &&
        mDialogueImage->GetColor()->Equals(float3(0.0f, 0.0f, 0.0f))) SetDebug(value);
}

void HudController::SetDialogue(bool value)
{
    value ? mDialogueImage->SetColor(float3(1.0f, 1.0f, 1.0f)) : mDialogueImage->SetColor(float3(0.0f, 0.0f, 0.0f));
    if (mGodmodeImage->GetColor()->Equals(float3(0.0f, 0.0f, 0.0f)) &&
        mInstakillImage->GetColor()->Equals(float3(0.0f, 0.0f, 0.0f))) SetDebug(value);
}

void HudController::SetDebug(bool value)
{
    mDebugGO->SetEnabled(value);
}

void HudController::SetHud(bool value)
{
    mHudGO->SetEnabled(value);
}

void HudController::SetAmmo(int ammo)
{
   if (mAmmoText) mAmmoText->SetText(std::to_string(ammo));
}

void HudController::SetEnergy(int energy, EnergyType type, bool up)
{
    if (mAmmoText) mAmmoText->SetText(std::to_string(energy));
    if (energy == 0) GameManager::GetInstance()->GetPlayerController()->GetWeapon()->SetCurrentAmmo(0);

    if (up)
    {
        mGunHLTimer.Reset();
        mGunHL = true;
        mGunHLGO->SetEnabled(true);
    }

    mAmmoBaseGO->SetEnabled(false);
    mAmmoSubGO->SetEnabled(false);
    mAmmoShotgunGO->SetEnabled(false);

    switch (type)
    {
    case EnergyType::NONE:
        mAmmoBaseGO->SetEnabled(true);
        break;
    case EnergyType::BLUE:
        mAmmoSubGO->SetEnabled(true);
        break;
    case EnergyType::RED:
        mAmmoShotgunGO->SetEnabled(true);
        break;
    default:
        mAmmoBaseGO->SetEnabled(true);
        break;
    }
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
    /*float newWidth = health * 2.8f;

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
    }*/
}

void HudController::SetGrenadeCooldown(float cooldown)
{
    mGrenadeImage->SetAlpha(0.5f);
    mGrenadeCooldown = cooldown;
    mGrenadeTimer = 0.001f;
}

void HudController::SetUltimateCooldown(float cooldown)
{
    mUltimateSlider->SetValue(cooldown / 100.0f);

    if (cooldown == 100)
    {
        mUltimateHL = true;
        mUltimateHLGO->SetEnabled(true);
        mUltimateSliderGO->SetEnabled(false);
        mUltimateImage->SetAlpha(1.0f);
    }
    else 
    {
        mUltimateHL = false;
        mUltimateHLGO->SetEnabled(false);
        mUltimateSliderGO->SetEnabled(true);
        mUltimateImage->SetAlpha(0.5f);
    }
}

void HudController::SetCollectibleText(std::string text, std::string title, std::string subtitle)
{
    if (mLoreText) mLoreText->SetText(text);
    if (mTitleText) mTitleText->SetText(title);
    if (mSubtitleText) mSubtitleText->SetText(subtitle);
}

void HudController::SetScreen(SCREEN name, bool active)
{
    switch (name) {
        case SCREEN::LOAD:
            if (mLoadingScreen) mLoadingScreen->SetEnabled(active);
            mLoadlevel = true;
            break;
        case SCREEN::LOSE:
            if (mLoseScreen)
            {
                mLoseScreen->SetEnabled(active);
                LoseAnimation();
            }
            break;
        case SCREEN::WIN:
            if (mWinScreen) mWinScreen->SetEnabled(active);
            break;
        case SCREEN::PAUSE:
            if (active) LOG("Pause true") else LOG("Pause false")
            mPauseMenu->Reset();
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
        mInteractGO->SetEnabled(active);
    }
        
}

void HudController::SetPickupPrompt(bool active)
{
    if (mPickupControllerGO && mPickupKeyboardGO)
    {
        if (GameManager::GetInstance()->UsingController()) 
        {
            mPickupControllerGO->SetEnabled(active);
        }
        else
        {
            mPickupKeyboardGO->SetEnabled(active);
        }
    }
}

#pragma region Click Events

void HudController::OnWinButtonClick()
{
    GameManager::GetInstance()->LoadLevel("Assets/Scenes/MainMenu");
    App->GetScene()->GetPlayerStats()->ResetStats();
}

void HudController::OnLoseButtonClick()
{
    GameManager::GetInstance()->LoadLevel("Assets/Scenes/MainMenu");
    App->GetScene()->GetPlayerStats()->ResetStats();
}

void HudController::OnTryAgainButtonClick()
{
    if (App->GetScene()->GetName() == "Level1Scene")
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level1Scene");
    }

    if (App->GetScene()->GetName() == "Level2Scene")
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level2Scene");
    }

    if (App->GetScene()->GetName() == "Level3Scene")
    {
        GameManager::GetInstance()->LoadLevel("Assets/Scenes/Level3Scene");
    }

    App->GetScene()->GetPlayerStats()->TryAgainStats();
}

void HudController::OnVideoBackClick()
{
    mVideoGO->SetEnabled(false);
    mVideoComponent->Stop();
    mIsVideoPlaying = false;
    SetDialog();
    ReleaseVideoAssociatedAudio();

    GameManager::GetInstance()->PauseBackgroundAudio(false);
}

void HudController::OnTryAgainButtonHoverOn()
{
    mTryAgainBtnClicked->SetEnabled(true);
}

void HudController::OnTryAgainButtonHoverOff()
{
    mTryAgainBtnClicked->SetEnabled(false);
}

void HudController::OnLoseButtonHoverOn() 
{
    mLoseMenuBtnClicked->SetEnabled(true);
}

void HudController::OnLoseButtonHoverOff() 
{
    mLoseMenuBtnClicked->SetEnabled(false);
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

void HudController::SetBossInvulnerable(bool value)
{
    LOG("Change color: " + value)
    if (value)
    {
        mBossHealthImage->SetColor(float3(77.0f / 255.f, 94.0f / 255.f, 114.0f / 255.f));
    }
    else 
    {
        mBossHealthImage->SetColor(float3(252.0f / 255.f, 15.0f / 255.f, 62.0f / 255.f));
    }
}

void HudController::ChangeBindings(bool controller)
{
    if (controller)
    {
        if (mControllerWeaponBinding) mControllerWeaponBinding->SetEnabled(true);
        if (mControllerUltiBinding) mControllerUltiBinding->SetEnabled(true);
        if (mControllerGrenadeBinding) mControllerGrenadeBinding->SetEnabled(true);
        if (mControllerDialogueNextBinding) mControllerDialogueNextBinding->SetEnabled(true);
        if (mControllerDialogueSkipBinding) mControllerDialogueSkipBinding->SetEnabled(true);
        if (mControllerVideoBinding) mControllerVideoBinding->SetEnabled(true);
        if (mControllerCollectibleOpenBinding) mControllerCollectibleOpenBinding->SetEnabled(true);
        if (mControllerCollectibleCloseBinding) mControllerCollectibleCloseBinding->SetEnabled(true);
        if (mControllerMenuSelectBinding) mControllerMenuSelectBinding->SetEnabled(true);
        if (mControllerMenuBackBinding) mControllerMenuBackBinding->SetEnabled(true);
        if (mControllerCreditsSkipBinding)  mControllerCreditsSkipBinding->SetEnabled(true);
        if (mControllerPresentationSkipBinding) mControllerPresentationSkipBinding->SetEnabled(true);
        if (mKeyboardWeaponBinding)  mKeyboardWeaponBinding->SetEnabled(false);
        if (mKeyboardUltiBinding)  mKeyboardUltiBinding->SetEnabled(false);
        if (mKeyboardGrenadeBinding) mKeyboardGrenadeBinding->SetEnabled(false);
        if (mKeyboardDialogueNextBinding) mKeyboardDialogueNextBinding->SetEnabled(false);
        if (mKeyboardDialogueSkipBinding)  mKeyboardDialogueSkipBinding->SetEnabled(false);
        if (mKeyboardVideoBinding)  mKeyboardVideoBinding->SetEnabled(false);
        if (mKeyboardCollectibleOpenBinding)  mKeyboardCollectibleOpenBinding->SetEnabled(false);
        if (mKeyboardCollectibleCloseBinding)  mKeyboardCollectibleCloseBinding->SetEnabled(false);
        if (mKeyboardMenuSelectBinding)  mKeyboardMenuSelectBinding->SetEnabled(false);
        if (mKeyboardMenuBackBinding)  mKeyboardMenuBackBinding->SetEnabled(false);
        if (mKeyboardCreditsSkipBinding)   mKeyboardCreditsSkipBinding->SetEnabled(false);
        if (mKeyboardPresentationSkipBinding) mKeyboardPresentationSkipBinding->SetEnabled(false);
    }
    else
    {
        if (mControllerWeaponBinding) mControllerWeaponBinding->SetEnabled(false);
        if (mControllerUltiBinding) mControllerUltiBinding->SetEnabled(false);
        if (mControllerGrenadeBinding) mControllerGrenadeBinding->SetEnabled(false);
        if (mControllerDialogueNextBinding) mControllerDialogueNextBinding->SetEnabled(false);
        if (mControllerDialogueSkipBinding) mControllerDialogueSkipBinding->SetEnabled(false);
        if (mControllerVideoBinding) mControllerVideoBinding->SetEnabled(false);
        if (mControllerCollectibleOpenBinding) mControllerCollectibleOpenBinding->SetEnabled(false);
        if (mControllerCollectibleCloseBinding) mControllerCollectibleCloseBinding->SetEnabled(false);
        if (mControllerMenuSelectBinding) mControllerMenuSelectBinding->SetEnabled(false);
        if (mControllerMenuBackBinding) mControllerMenuBackBinding->SetEnabled(false);
        if (mControllerCreditsSkipBinding)  mControllerCreditsSkipBinding->SetEnabled(false);
        if (mControllerPresentationSkipBinding) mControllerPresentationSkipBinding->SetEnabled(false);
        if (mKeyboardWeaponBinding)  mKeyboardWeaponBinding->SetEnabled(true);
        if (mKeyboardUltiBinding)  mKeyboardUltiBinding->SetEnabled(true);
        if (mKeyboardGrenadeBinding) mKeyboardGrenadeBinding->SetEnabled(true);
        if (mKeyboardDialogueNextBinding) mKeyboardDialogueNextBinding->SetEnabled(true);
        if (mKeyboardDialogueSkipBinding)  mKeyboardDialogueSkipBinding->SetEnabled(true);
        if (mKeyboardVideoBinding)  mKeyboardVideoBinding->SetEnabled(true);
        if (mKeyboardCollectibleOpenBinding)  mKeyboardCollectibleOpenBinding->SetEnabled(true);
        if (mKeyboardCollectibleCloseBinding)  mKeyboardCollectibleCloseBinding->SetEnabled(true);
        if (mKeyboardMenuSelectBinding)  mKeyboardMenuSelectBinding->SetEnabled(true);
        if (mKeyboardMenuBackBinding)  mKeyboardMenuBackBinding->SetEnabled(true);
        if (mKeyboardCreditsSkipBinding)   mKeyboardCreditsSkipBinding->SetEnabled(true);
        if (mKeyboardPresentationSkipBinding) mKeyboardPresentationSkipBinding->SetEnabled(true);
    }
}

#pragma endregion