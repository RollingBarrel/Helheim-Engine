#include "MainMenu.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "ModuleAudio.h"
#include "ScriptComponent.h"
#include "MainMenuManager.h"
#include "Transform2DComponent.h"
#include "ButtonComponent.h"
#include "ImageComponent.h"
#include "Resource.h"
#include "ModuleResource.h"
#include "TextComponent.h"
#include "Timer.h"
#include "ModuleWindow.h"
#include "AudioManager.h"

CREATE(MainMenu)
{
    CLASS(owner);

    SEPARATOR("MENUS");
    MEMBER(MemberType::GAMEOBJECT, mSplashScreen);
    MEMBER(MemberType::GAMEOBJECT, mStudioScreen);
    MEMBER(MemberType::GAMEOBJECT, mEngineScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoadingScreen);

    MEMBER(MemberType::GAMEOBJECT, mMainMenu);
    MEMBER(MemberType::GAMEOBJECT, mOptionsMenu);
    MEMBER(MemberType::GAMEOBJECT, mCreditsMenu);
    MEMBER(MemberType::GAMEOBJECT, mCreditsText);
    MEMBER(MemberType::GAMEOBJECT, mOptionsContainerGO);
    MEMBER(MemberType::GAMEOBJECT, mControlsGO);
    MEMBER(MemberType::GAMEOBJECT, mSettingsGO);
    MEMBER(MemberType::GAMEOBJECT, mControlsButtonGO);
    MEMBER(MemberType::GAMEOBJECT, mSettingsButtonGO);

    SEPARATOR("SETTINGS");
    MEMBER(MemberType::GAMEOBJECT, mGeneralVolumeButtonGO);
    MEMBER(MemberType::GAMEOBJECT, mMusicVolumeButtonGO);
    MEMBER(MemberType::GAMEOBJECT, mEffectsVolumeButtonGO);
    MEMBER(MemberType::GAMEOBJECT, mVSyncButtonGO);
    MEMBER(MemberType::GAMEOBJECT, mFullscreenButtonGO);

    SEPARATOR("BUTTONS");
    MEMBER(MemberType::GAMEOBJECT, mPlayGO);
    MEMBER(MemberType::GAMEOBJECT, mOptionsGO);
    MEMBER(MemberType::GAMEOBJECT, mCreditsGO);
    MEMBER(MemberType::GAMEOBJECT, mQuitGO);
    MEMBER(MemberType::GAMEOBJECT, mBackCreditGO);

    SEPARATOR("OTHERS");
    MEMBER(MemberType::GAMEOBJECT, mAudioManagerGO);

    END_CREATE;
}

MainMenu::MainMenu(GameObject* owner) : Script(owner) {}

void MainMenu::Start() 
{
    mSplashButton = static_cast<ButtonComponent*>(mSplashScreen->GetComponent(ComponentType::BUTTON));
    mPlayButton = static_cast<ButtonComponent*>(mPlayGO->GetComponent(ComponentType::BUTTON));
    mOptionsButton = static_cast<ButtonComponent*>(mOptionsGO->GetComponent(ComponentType::BUTTON));
    mCreditsButton = static_cast<ButtonComponent*>(mCreditsGO->GetComponent(ComponentType::BUTTON));
    mQuitButton = static_cast<ButtonComponent*>(mQuitGO->GetComponent(ComponentType::BUTTON));
    mBackCreditButton = static_cast<ButtonComponent*>(mBackCreditGO->GetComponent(ComponentType::BUTTON));
    mSettingsButton = static_cast<ButtonComponent*>(mSettingsButtonGO->GetComponent(ComponentType::BUTTON));
    mControlsButton = static_cast<ButtonComponent*>(mControlsButtonGO->GetComponent(ComponentType::BUTTON));

    mSplashButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnSplashButtonClick, this)));
    mPlayButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnNewButtonClick, this)));
    mOptionsButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnOptionsButtonClick, this)));
    mCreditsButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnCreditsButtonClick, this)));
    mQuitButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnQuitButtonClick, this)));
    mBackCreditButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnMainButtonClick, this)));
    mBackCreditButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&MainMenu::OnBackButtonHover, this)));
    mBackCreditButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&MainMenu::OnBackButtonHoverOff, this)));

    mPlayButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&MainMenu::OnPlayButtonHover, this)));
    mOptionsButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&MainMenu::OnOptionsButtonHover, this)));
    mCreditsButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&MainMenu::OnCreditsButtonHover, this)));
    mQuitButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&MainMenu::OnQuitButtonHover, this)));

    mPlayButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&MainMenu::OnPlayButtonHoverOff, this)));
    mOptionsButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&MainMenu::OnOptionsButtonHoverOff, this)));
    mCreditsButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&MainMenu::OnCreditsButtonHoverOff, this)));
    mQuitButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&MainMenu::OnQuitButtonHoverOff, this)));

    mSettingsButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnSettingsButtonClick, this)));
    mControlsButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnControlsButtonClick, this)));
    mSettingsButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&MainMenu::OnSettingsButtonHover, this)));
    mControlsButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&MainMenu::OnControlsButtonHover, this)));
    mSettingsButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&MainMenu::OnSettingsButtonHoverOff, this)));
    mControlsButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&MainMenu::OnControlsButtonHoverOff, this)));

    mTextTransform = static_cast<Transform2DComponent*>(mCreditsText->GetComponent(ComponentType::TRANSFORM2D));

    mMusicVolumeButton = static_cast<ButtonComponent*>(mMusicVolumeButtonGO->GetComponent(ComponentType::BUTTON));
    mGeneralVolumeButton = static_cast<ButtonComponent*>(mGeneralVolumeButtonGO->GetComponent(ComponentType::BUTTON));
    mEffectsVolumeButton = static_cast<ButtonComponent*>(mEffectsVolumeButtonGO->GetComponent(ComponentType::BUTTON));
    mVSyncButton = static_cast<ButtonComponent*>(mVSyncButtonGO->GetComponent(ComponentType::BUTTON));
    mFullscreenButton = static_cast<ButtonComponent*>(mFullscreenButtonGO->GetComponent(ComponentType::BUTTON));
    mVSyncImage = static_cast<ImageComponent*>(mVSyncButtonGO->GetComponent(ComponentType::IMAGE));
    mFullscreenImage = static_cast<ImageComponent*>(mFullscreenButtonGO->GetComponent(ComponentType::IMAGE));


    mGeneralVolumeButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnGeneralVolumeButtonClick, this)));
    mMusicVolumeButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnMusicVolumeButtonClick, this)));
    mEffectsVolumeButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnEffectsVolumeButtonClick, this)));
    mVSyncButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnVSyncButtonClick, this)));
    mFullscreenButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnFullscreenButtonClick, this)));

    if (mAudioManagerGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mAudioManagerGO->GetComponent(ComponentType::SCRIPT));
        mAudioManager = static_cast<AudioManager*>(script->GetScriptInstance());
        mBGMID = mAudioManager->Play(BGM::MAINMENU);
    }

    OpenMenu(MENU_TYPE::STUDIO);
}

void MainMenu::Update()
{
    Controls();
    if (mIsInitial) 
    {
        if (Delay(2.0f) && mCurrentMenu == MENU_TYPE::STUDIO) OpenMenu(MENU_TYPE::ENGINE);
        else if (Delay(2.0f) && mCurrentMenu == MENU_TYPE::ENGINE)
        {
            OpenMenu(MENU_TYPE::SPLASH);
        }
        return;
    }

    if (mIsScrolling)
    {
        float3 currentPosition = mTextTransform->GetPosition();
        if (currentPosition.y > 3400.0f) mTextTransform->SetPosition(float3(currentPosition.x, -500.0f, currentPosition.z));
        else mTextTransform->SetPosition(float3(currentPosition.x, currentPosition.y + 200 * App->GetDt(), currentPosition.z));
    }

    if (mLoadlevel == true && Delay(1.0f)) 
    {
        mAudioManager->Release(BGM::MAINMENU, mBGMID);
        App->GetScene()->Load("Assets/Scenes/Level1Scene");
    }

}


bool MainMenu::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}

void MainMenu::Controls()
{
    if (App->GetInput()->GetKey(Keys::Keys_UP) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP) == ButtonState::BUTTON_DOWN) 
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
        if (mCurrentMenu == MENU_TYPE::MAIN) // MENU MAIN BUTTONS
        { 
            if (mOption > 0)
            {
                mOption--;
            }
            else
            {
                mOption = 3;
            }
            HoverMenu(static_cast<MENU_TYPE>(mOption));
        }
        else if (mCurrentMenu == MENU_TYPE::OPTIONS)
        {
            if (mSettingOption > 7)
            {
                mSettingOption--;
            }
            else
            {
                mSettingOption = 8;
            }
            HoverSubMenu(static_cast<MENU_TYPE>(mSettingOption));
        }
        else if (mCurrentMenu == MENU_TYPE::SETTINGS)
        {
            if (mSubsettingOption > 0)
            {
                mSubsettingOption--;
            }
            else
            {
                mSubsettingOption = 4;
            }
            HoverSubSubMenu(static_cast<SETTING_TYPE>(mSubsettingOption));
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_DOWN) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN) == ButtonState::BUTTON_DOWN)
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
        if (mCurrentMenu == MENU_TYPE::MAIN) // MENU MAIN BUTTONS
        {
            if (mOption < 3)
            {
                mOption++;
            }
            else
            {
                mOption = 0;
            }
            HoverMenu(static_cast<MENU_TYPE>(mOption));
        }
        else if (mCurrentMenu == MENU_TYPE::OPTIONS)
        {
            if (mSettingOption < 8)

            {
                mSettingOption++;
            }
            else
            {
                mSettingOption = 7;
            }
            HoverSubMenu(static_cast<MENU_TYPE>(mSettingOption));
        }
        else if (mCurrentMenu == MENU_TYPE::SETTINGS) 
        {
            if (mSubsettingOption < 4)
            {
                mSubsettingOption++;
            }
            else
            {
                mSubsettingOption = 0;
            }
                HoverSubSubMenu(static_cast<SETTING_TYPE>(mSubsettingOption));
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN || 
        App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
    {
        if (mIsInitial) 
        {
            OpenMenu(MENU_TYPE::MAIN);
            mIsInitial = false;
            return;
        }
        if (mCurrentMenu == MENU_TYPE::MAIN)
        {
            ClickMenu(static_cast<MENU_TYPE>(mOption));
        } 
        else if (mCurrentMenu == MENU_TYPE::OPTIONS)
        {
            OpenMenu(static_cast<MENU_TYPE>(mSettingOption));
        }
        else if (mCurrentMenu == MENU_TYPE::SETTINGS)
        {
            if (mCurrentSetting == SETTING_TYPE::VSYNC)
            {
                OnVSyncButtonClick();
            }
            else if (mCurrentSetting == SETTING_TYPE::VSYNC)
            {
				OnFullscreenButtonClick();
            }
            
        }
    }

    /*
     *  TODO: Add right and left control to handle the settings that use sliders
     *  Maybe we could also add left and right shoulder buttons to increase/decrease by 10
    */

    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_DOWN)
    {
        if (mCurrentMenu == MENU_TYPE::SETTINGS || mCurrentMenu == MENU_TYPE::CONTROLS)
        {
			if (mCurrentMenu == MENU_TYPE::SETTINGS)
            {
                mSubsettingOption = 0;
			}
			mSettingOption = 7; // Reset the setting option
            OpenMenu(MENU_TYPE::OPTIONS);
        }
		else
        {
            mOption = 0; // Reset the option
			OpenMenu(MENU_TYPE::MAIN);
		}
    }
}

// MENUS

void MainMenu::OpenMenu(MENU_TYPE type) 
{
    mCurrentMenu = type;

    mEngineScreen->SetEnabled(false);
    mStudioScreen->SetEnabled(false);
    mSplashScreen->SetEnabled(false);
    mLoadingScreen->SetEnabled(false);

    mOptionsMenu->SetEnabled(false);
    mCreditsMenu->SetEnabled(false);

    mSettingsGO->SetEnabled(false);
    mControlsGO->SetEnabled(false);
    mOptionsContainerGO->SetEnabled(false);

    mIsScrolling = false;


    switch (type) 
    {
        case MENU_TYPE::MAIN:
            mMainMenu->SetEnabled(true);
            mIsInitial = false;
			OnPlayButtonHover(); // Hover the first button
            break;
        case MENU_TYPE::OPTIONS:
            mOptionsMenu->SetEnabled(true);
            mOptionsContainerGO->SetEnabled(true);
			OnControlsButtonHover();
            break;
        case MENU_TYPE::CREDITS:
            mCreditsMenu->SetEnabled(true);
            mIsScrolling = true;
            mTextTransform->SetPosition(float3(mTextTransform->GetPosition().x, 0, mTextTransform->GetPosition().z));
            break;
        case MENU_TYPE::LOADING:
            mLoadingScreen->SetEnabled(true);
            break;
        case MENU_TYPE::SPLASH:
            mSplashScreen->SetEnabled(true);
            break;
        case MENU_TYPE::ENGINE:
            mEngineScreen->SetEnabled(true);
            break;
        case MENU_TYPE::STUDIO:
            mStudioScreen->SetEnabled(true);
            break;
        case MENU_TYPE::CONTROLS:
            mOptionsMenu->SetEnabled(true);
            mOptionsContainerGO->SetEnabled(true);
            mControlsGO->SetEnabled(true);
            break;
        case MENU_TYPE::SETTINGS:
            mOptionsMenu->SetEnabled(true);
            mOptionsContainerGO->SetEnabled(true);
            mSettingsGO->SetEnabled(true);
            OnGeneralVolumeButtonHover(); // Hover the first button
            break;
    }
}

// CLICKS

void MainMenu::ClickMenu(MENU_TYPE type)
{
    if (mSplashScreen->IsActive())
    {
        OnSplashButtonClick();
        return;
    }

    switch (type) {
    case MENU_TYPE::MAIN:
        OnNewButtonClick();
        break;
    case MENU_TYPE::OPTIONS:
        OnOptionsButtonClick();
        break;
    case MENU_TYPE::CREDITS:
        OnCreditsButtonClick();
        break;
    case MENU_TYPE::LOADING:
        OnQuitButtonClick();
        break;
    }
}

void MainMenu::OnMainButtonClick() 
{
    mAudioManager->PlayOneShot(SFX::MAINMENU_OK);
    OpenMenu(MENU_TYPE::MAIN);
}

void MainMenu::OnQuitButtonClick() {
    App->Exit();
    mAudioManager->PlayOneShot(SFX::MAINMENU_OK);
    exit(0);
}

void MainMenu::OnOptionsButtonClick() 
{
    mAudioManager->PlayOneShot(SFX::MAINMENU_OK);
    OpenMenu(MENU_TYPE::OPTIONS);
}

void MainMenu::OnCreditsButtonClick() 
{
    mAudioManager->PlayOneShot(SFX::MAINMENU_OK);
    OpenMenu(MENU_TYPE::CREDITS);
}

void MainMenu::OnNewButtonClick() 
{
    mAudioManager->PlayOneShot(SFX::MAINMENU_OK);
    OpenMenu(MENU_TYPE::LOADING);
    mLoadlevel = true;
}

void MainMenu::OnSplashButtonClick() 
{
    OnPlayButtonHover();
    mAudioManager->PlayOneShot(SFX::MAINMENU_OK);
    OpenMenu(MENU_TYPE::MAIN);
}

void MainMenu::OnControlsButtonClick()
{
	mSettingsGO->SetEnabled(false);
    mAudioManager->PlayOneShot(SFX::MAINMENU_OK);
    OpenMenu(MENU_TYPE::CONTROLS);
}

void MainMenu::OnSettingsButtonClick()
{
        mAudioManager->PlayOneShot(SFX::MAINMENU_OK);
        OpenMenu(MENU_TYPE::SETTINGS);
}

void MainMenu::OnVSyncButtonClick()
{
    if (*mVSyncImage->GetAlpha() == 1.0f)
    {
        mVSyncImage->SetAlpha(0.5f);
        App->GetCurrentClock()->SetVsyncStatus(false);
    }
    else
    {
        mVSyncImage->SetAlpha(1.0f);
        App->GetCurrentClock()->SetVsyncStatus(true);
    }
}

//TODO: Investige why this is not working.
void MainMenu::OnFullscreenButtonClick()
{
    if (*mFullscreenImage->GetAlpha() == 1.0f)
    {
        mFullscreenImage->SetAlpha(0.5f);
        App->GetWindow()->WindowFullscreen(false);
    }
    else
    {
        mFullscreenImage->SetAlpha(1.0f);
        App->GetWindow()->WindowFullscreen(true);
    }
}

void MainMenu::OnGeneralVolumeButtonClick()
{
	LOG("General Volume");
}

void MainMenu::OnMusicVolumeButtonClick() {
    LOG("Music Volume");
}
void MainMenu::OnEffectsVolumeButtonClick() {
    LOG("Effects Volume");
}

// SELECTED

void MainMenu::HoverMenu(MENU_TYPE type) 
{
    switch (type) 
    {
        case MENU_TYPE::MAIN:
            OnPlayButtonHover();
            break;
        case MENU_TYPE::OPTIONS:
            OnOptionsButtonHover();
            break;
        case MENU_TYPE::CREDITS:
            OnCreditsButtonHover();
            break;
        case MENU_TYPE::LOADING:
            OnQuitButtonHover();
            break;
    }
}

void MainMenu::HoverSubMenu(MENU_TYPE type)
{
    switch (type) 
    {
        case MENU_TYPE::CONTROLS:
            OnControlsButtonHover();
            break;
        case MENU_TYPE::SETTINGS:
            OnSettingsButtonHover();
            break;
    }
}

void MainMenu::HoverSubSubMenu(SETTING_TYPE type)
{
    switch (type)
    {
    case SETTING_TYPE::GENERAL_VOLUME:
        OnGeneralVolumeButtonHover();
        break;
    case SETTING_TYPE::MUSIC_VOLUME:
        OnMusicVolumeButtonHover();
        break;
    case SETTING_TYPE::EFFECTS_VOLUME:
        OnEffectsVolumeButtonHover();
        break;
    case SETTING_TYPE::VSYNC:
        OnVSyncButtonHover();
		break;
    case SETTING_TYPE::FULL_SCREEN:
        OnFullscreenButtonHover();
		break;
    }
}

void MainMenu::OnGeneralVolumeButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mGeneralVolumeButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(1.f);
	mCurrentSetting = SETTING_TYPE::GENERAL_VOLUME;

	//TODO: Abstract this abomination (in all the hover functions)
    OnMusicVolumeButtonHoverOff();
    OnEffectsVolumeButtonHoverOff();
    OnVSyncButtonHoverOff();
    OnFullscreenButtonHoverOff();
}

void MainMenu::OnMusicVolumeButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mMusicVolumeButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(1.f);
    mCurrentSetting = SETTING_TYPE::MUSIC_VOLUME;

    OnGeneralVolumeButtonHoverOff();
    OnEffectsVolumeButtonHoverOff();
    OnVSyncButtonHoverOff();
    OnFullscreenButtonHoverOff();
}

void MainMenu::OnEffectsVolumeButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mEffectsVolumeButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(1.f);
    mCurrentSetting = SETTING_TYPE::EFFECTS_VOLUME;

    OnGeneralVolumeButtonHoverOff();
    OnMusicVolumeButtonHoverOff();
    OnVSyncButtonHoverOff();
    OnFullscreenButtonHoverOff();
}

void MainMenu::OnVSyncButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mVSyncButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(1.f);
    mCurrentSetting = SETTING_TYPE::VSYNC;

    OnGeneralVolumeButtonHoverOff();
    OnMusicVolumeButtonHoverOff();
    OnEffectsVolumeButtonHoverOff();
    OnFullscreenButtonHoverOff();
}

void MainMenu::OnFullscreenButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mFullscreenButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(1.f);
    mCurrentSetting = SETTING_TYPE::FULL_SCREEN;

    OnGeneralVolumeButtonHoverOff();
    OnMusicVolumeButtonHoverOff();
    OnEffectsVolumeButtonHoverOff();
    OnVSyncButtonHoverOff();
}

void MainMenu::OnQuitButtonHover() 
{
    if (mOption != 3) 
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
    }
    ImageComponent* image = static_cast<ImageComponent*>(mQuitGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
    mOption = 3;

    // Set the other hovers off (integration mouse/click)
    OnCreditsButtonHoverOff();
    OnPlayButtonHoverOff();
    OnOptionsButtonHoverOff();
}

void MainMenu::OnQuitButtonHoverOff() 
{
    ImageComponent* image = static_cast<ImageComponent*>(mQuitGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

void MainMenu::OnOptionsButtonHover() 
{
    if (mOption != 1)
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
    }
    ImageComponent* image = static_cast<ImageComponent*>(mOptionsGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
    mOption = 1;

    // Set the other hovers off (integration mouse/click)
    OnCreditsButtonHoverOff();
    OnPlayButtonHoverOff();
    OnQuitButtonHoverOff();
}

void MainMenu::OnOptionsButtonHoverOff() 
{
    ImageComponent* image = static_cast<ImageComponent*>(mOptionsGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

void MainMenu::OnCreditsButtonHover() 
{
    if (mOption != 2)
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
    }
    ImageComponent* image = static_cast<ImageComponent*>(mCreditsGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
    mOption = 2;

    // Set the other hovers off (integration mouse/click)
    OnPlayButtonHoverOff();
    OnOptionsButtonHoverOff();
    OnQuitButtonHoverOff();
}

void MainMenu::OnCreditsButtonHoverOff() 
{
    ImageComponent* image = static_cast<ImageComponent*>(mCreditsGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

void MainMenu::OnPlayButtonHover()
{
    if (mOption != 0)
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
    }
    ImageComponent* image = static_cast<ImageComponent*>(mPlayGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
    mOption = 0;

    // Set the other hovers off (integration mouse/click)
    OnCreditsButtonHoverOff();
    OnOptionsButtonHoverOff();
    OnQuitButtonHoverOff();
}

void MainMenu::OnPlayButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mPlayGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

void MainMenu::OnBackCreditsButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mBackCreditGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
    mOption = 0;
}

void MainMenu::OnBackCreditsButtonHoverOff() 
{
    ImageComponent* image = static_cast<ImageComponent*>(mBackCreditGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

void MainMenu::OnControlsButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mControlsButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);

    // Set the other hovers off (integration mouse/click)
    OnSettingsButtonHoverOff();
}

void MainMenu::OnControlsButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mControlsButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.2f);
}

void MainMenu::OnSettingsButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mSettingsButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
    // Set the other hovers off (integration mouse/click)
    OnControlsButtonHoverOff();
}

void MainMenu::OnSettingsButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mSettingsButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.2f);
}

void MainMenu::OnBackButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mBackCreditGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
}

void MainMenu::OnBackButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mBackCreditGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.0f);
}

void MainMenu::OnGeneralVolumeButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mGeneralVolumeButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
}

void MainMenu::OnMusicVolumeButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mMusicVolumeButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
}

void MainMenu::OnEffectsVolumeButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mEffectsVolumeButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
}

void MainMenu::OnVSyncButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mVSyncButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
}

void MainMenu::OnFullscreenButtonHoverOff()
{
    ImageComponent* image = static_cast<ImageComponent*>(mFullscreenButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(0.8f);
}

