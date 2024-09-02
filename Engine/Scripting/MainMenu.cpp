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
#include "SliderComponent.h"
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
	MEMBER(MemberType::GAMEOBJECT, mGeneralVolumeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mGeneralVolumeFillGO);
    MEMBER(MemberType::GAMEOBJECT, mMusicVolumeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mMusicVolumeFillGO);
    MEMBER(MemberType::GAMEOBJECT, mEffectsVolumeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mEffectsVolumeFillGO);
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

    mGeneralVolumeSlider = static_cast<SliderComponent*>(mGeneralVolumeSliderGO->GetComponent(ComponentType::SLIDER));
    mGeneralVolumeFill = static_cast<ImageComponent*>(mGeneralVolumeFillGO->GetComponent(ComponentType::IMAGE));
    mMusicVolumeSlider = static_cast<SliderComponent*>(mMusicVolumeSliderGO->GetComponent(ComponentType::SLIDER));
    mMusicVolumeFill = static_cast<ImageComponent*>(mMusicVolumeFillGO->GetComponent(ComponentType::IMAGE));
    mEffectsVolumeSlider = static_cast<SliderComponent*>(mEffectsVolumeSliderGO->GetComponent(ComponentType::SLIDER));
    mEffectsVolumeFill = static_cast<ImageComponent*>(mEffectsVolumeFillGO->GetComponent(ComponentType::IMAGE));

    mVSyncButton = static_cast<ButtonComponent*>(mVSyncButtonGO->GetComponent(ComponentType::BUTTON));
    mFullscreenButton = static_cast<ButtonComponent*>(mFullscreenButtonGO->GetComponent(ComponentType::BUTTON));
    mVSyncImage = static_cast<ImageComponent*>(mVSyncButtonGO->GetComponent(ComponentType::IMAGE));
    mFullscreenImage = static_cast<ImageComponent*>(mFullscreenButtonGO->GetComponent(ComponentType::IMAGE));

    mVSyncButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnVSyncButtonClick, this)));
    mFullscreenButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnFullscreenButtonClick, this)));

    if (mAudioManagerGO)
    {
        ScriptComponent* script = static_cast<ScriptComponent*>(mAudioManagerGO->GetComponent(ComponentType::SCRIPT));
        mAudioManager = static_cast<AudioManager*>(script->GetScriptInstance());
        mBGMID = mAudioManager->Play(BGM::MAINMENU);
    }

    // Init the volume setting sliders
    App->GetAudio()->SetVolume("bus:/", mGeneralVolumeValue);
    mGeneralVolumeSlider->SetValue(mGeneralVolumeValue);
    App->GetAudio()->SetVolume("bus:/music", mMusicVolumeValue);
    mMusicVolumeSlider->SetValue(mMusicVolumeValue);
    App->GetAudio()->SetVolume("bus:/sfx", mEffectsVolumeValue);
    mEffectsVolumeSlider->SetValue(mEffectsVolumeValue);


    OpenMenu(MENU_TYPE::STUDIO);
}

void MainMenu::Update()
{
    if (mStudioBool) {
        if (mStudioTimer.DelayWithoutReset(2.0f))
        {
            OpenMenu(MENU_TYPE::ENGINE);
            mStudioBool = false;
            return;
        }
        else return;
    }
        
    if (mEngineBool) {
        if (mEngineTimer.DelayWithoutReset(2.0f))
        {
            OpenMenu(MENU_TYPE::SPLASH);
            mEngineBool = false;
            return;
        }
        else return;
    }
        
    if (mIsInitial) 
    {
        if (App->GetInput()->GetKeyboardReciveInputs() ||
            App->GetInput()->GetGameControllerReceivedInputs() ||
            App->GetInput()->GetMouseRecieveInputs()) 
        {
            mIsInitial = false;
            OpenMenu(MENU_TYPE::MAIN);
            OnPlayButtonHover(); // Hover first option when the menu is first laoded    
            OnControlsButtonHover(); // Pre-hover the first option
            return;
        }
        else return;
    }

    Controls();
        

    if (mIsScrolling)
    {
        float3 currentPosition = mTextTransform->GetPosition();
        if (currentPosition.y > 3400.0f) mTextTransform->SetPosition(float3(currentPosition.x, -500.0f, currentPosition.z));
        else mTextTransform->SetPosition(float3(currentPosition.x, currentPosition.y + 200 * App->GetDt(), currentPosition.z));
    }

    if (mLoadlevel == true && mTimer.Delay(1.0f))
    {
        mAudioManager->Release(BGM::MAINMENU, mBGMID);
        App->GetScene()->Load("Assets/Scenes/Level1Scene");
    }

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
            else if (mCurrentSetting == SETTING_TYPE::FULL_SCREEN)
            {
				OnFullscreenButtonClick();
            }
            
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_LEFT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT) == ButtonState::BUTTON_DOWN)
    {
        if (mCurrentMenu == MENU_TYPE::SETTINGS)
        {
            OnSlide(static_cast<SETTING_TYPE>(mSubsettingOption), DIRECTION::LEFT, 0.01f);
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_RIGHT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == ButtonState::BUTTON_DOWN)
    {
        if (mCurrentMenu == MENU_TYPE::SETTINGS)
        {
            OnSlide(static_cast<SETTING_TYPE>(mSubsettingOption), DIRECTION::RIGHT, 0.01f);
        }
        
    }

    //TODO: Implement a way to increase/decrease x10 using the keyboard also
    if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_DOWN)
    {
        if (mCurrentMenu == MENU_TYPE::SETTINGS)
        {
            OnSlide(static_cast<SETTING_TYPE>(mSubsettingOption), DIRECTION::RIGHT, 0.1f);
        }

    }

    if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == ButtonState::BUTTON_DOWN)
    {
        if (mCurrentMenu == MENU_TYPE::SETTINGS)
        {
            OnSlide(static_cast<SETTING_TYPE>(mSubsettingOption), DIRECTION::LEFT, 0.1f);
        }

    }



    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_DOWN)
    {
        if (mCurrentMenu == MENU_TYPE::SETTINGS || mCurrentMenu == MENU_TYPE::CONTROLS)
        {
			if (mCurrentMenu == MENU_TYPE::SETTINGS)
            {
                mSubsettingOption = 0;
                OnSettingsButtonHover();
            }
            else {
                OnControlsButtonHover();
            }
            OpenMenu(MENU_TYPE::OPTIONS);
        }
		else
        {
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
            break;
        case MENU_TYPE::OPTIONS:
            mOptionsMenu->SetEnabled(true);
            mOptionsContainerGO->SetEnabled(true);
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
            OnGeneralVolumeHover(); // Hover the first button
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
    App->GetWindow()->WindowFullscreen(!App->GetWindow()->IsWindowFullscreen());
}

void MainMenu::OnSlide(SETTING_TYPE type, DIRECTION direction, float step)
{
	// Early return. Just double checking we're not calling it on elements that are not sliders within the settings menu.
    if (type != SETTING_TYPE::GENERAL_VOLUME && type != SETTING_TYPE::MUSIC_VOLUME && type != SETTING_TYPE::EFFECTS_VOLUME) return;

    switch (type)
    {
    case SETTING_TYPE::GENERAL_VOLUME:
		if (direction == DIRECTION::LEFT)
        {
            mGeneralVolumeValue = App->GetAudio()->GetVolume("bus:/");
            if (mGeneralVolumeValue - step <= 0.0f)
            {
                App->GetAudio()->SetVolume("bus:/", 0.f);
				mGeneralVolumeSlider->SetValue(0.001f);
            }
            else
			{
				App->GetAudio()->SetVolume("bus:/", mGeneralVolumeValue - step);
                mGeneralVolumeSlider->SetValue(mGeneralVolumeValue - step);
            }
		}
		else // DIRECTION::RIGHT
        {
            mGeneralVolumeValue = App->GetAudio()->GetVolume("bus:/");
            if (mGeneralVolumeValue + step > 1.f)
            {
                App->GetAudio()->SetVolume("bus:/", 1.f);
                mGeneralVolumeSlider->SetValue(1.f);
            }
            else
            {
                App->GetAudio()->SetVolume("bus:/", mGeneralVolumeValue + step);
                mGeneralVolumeSlider->SetValue(mGeneralVolumeValue + step);
            }
        }
        break;
    case SETTING_TYPE::MUSIC_VOLUME:
        if (direction == DIRECTION::LEFT)
        {
            mMusicVolumeValue = App->GetAudio()->GetVolume("bus:/music");
            if (mMusicVolumeValue - step <= 0.0f)
            {
                App->GetAudio()->SetVolume("bus:/music", 0.f);
                mMusicVolumeSlider->SetValue(0.001f);
            }
            else
            {
                App->GetAudio()->SetVolume("bus:/music", mMusicVolumeValue - step);
                mMusicVolumeSlider->SetValue(mMusicVolumeValue - step);
            }
        }
        else // DIRECTION::RIGHT
        {
            mMusicVolumeValue = App->GetAudio()->GetVolume("bus:/music");
            if (mMusicVolumeValue + step > 1.f)
            {
                App->GetAudio()->SetVolume("bus:/music", 1.f);
                mMusicVolumeSlider->SetValue(1.f);
            }
            else
            {
                App->GetAudio()->SetVolume("bus:/music", mMusicVolumeValue + step);
                mMusicVolumeSlider->SetValue(mMusicVolumeValue + step);
            }
        }
        break;
    case SETTING_TYPE::EFFECTS_VOLUME:
        if (direction == DIRECTION::LEFT)
        {
            mEffectsVolumeValue = App->GetAudio()->GetVolume("bus:/sfx");
            if (mEffectsVolumeValue - step <= 0.0f)
            {
                App->GetAudio()->SetVolume("bus:/sfx", 0.f);
                mEffectsVolumeSlider->SetValue(0.001f);
            }
            else
            {
                App->GetAudio()->SetVolume("bus:/sfx", mEffectsVolumeValue - step);
                mEffectsVolumeSlider->SetValue(mEffectsVolumeValue - step);
            }
        }
        else // DIRECTION::RIGHT
        {
            mEffectsVolumeValue = App->GetAudio()->GetVolume("bus:/sfx");
            if (mEffectsVolumeValue + step > 1.f)
            {
                App->GetAudio()->SetVolume("bus:/sfx", 1.f);
                mEffectsVolumeSlider->SetValue(1.f);
            }
            else
            {
                App->GetAudio()->SetVolume("bus:/sfx", mEffectsVolumeValue + step);
                mEffectsVolumeSlider->SetValue(mEffectsVolumeValue + step);
            }
        }
        break;
    }
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
        OnGeneralVolumeHover();
        break;
    case SETTING_TYPE::MUSIC_VOLUME:
        OnMusicVolumeHover();
        break;
    case SETTING_TYPE::EFFECTS_VOLUME:
        OnEffectsVolumeHover();
        break;
    case SETTING_TYPE::VSYNC:
        OnVSyncButtonHover();
		break;
    case SETTING_TYPE::FULL_SCREEN:
        OnFullscreenButtonHover();
		break;
    }
}



void MainMenu::OnGeneralVolumeHover()
{
    //ImageComponent* image = static_cast<ImageComponent*>(mGeneralVolumeButtonGO->GetComponent(ComponentType::IMAGE));
    mGeneralVolumeFill->SetAlpha(1.f);
	mCurrentSetting = SETTING_TYPE::GENERAL_VOLUME;

	//TODO: Abstract this abomination (in all of the hover functions)
    OnMusicVolumeHoverOff();
    OnEffectsVolumeHoverOff();
    OnVSyncButtonHoverOff();
    OnFullscreenButtonHoverOff();
}

void MainMenu::OnMusicVolumeHover()
{
    mMusicVolumeFill->SetAlpha(1.f);
    mCurrentSetting = SETTING_TYPE::MUSIC_VOLUME;

    OnGeneralVolumeHoverOff();
    OnEffectsVolumeHoverOff();
    OnVSyncButtonHoverOff();
    OnFullscreenButtonHoverOff();
}

void MainMenu::OnEffectsVolumeHover()
{
	mEffectsVolumeFill->SetAlpha(1.f);
    mCurrentSetting = SETTING_TYPE::EFFECTS_VOLUME;

    OnGeneralVolumeHoverOff();
    OnMusicVolumeHoverOff();
    OnVSyncButtonHoverOff();
    OnFullscreenButtonHoverOff();
}

void MainMenu::OnVSyncButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mVSyncButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(1.f);
    mCurrentSetting = SETTING_TYPE::VSYNC;

    OnGeneralVolumeHoverOff();
    OnMusicVolumeHoverOff();
    OnEffectsVolumeHoverOff();
    OnFullscreenButtonHoverOff();
}

void MainMenu::OnFullscreenButtonHover()
{
    ImageComponent* image = static_cast<ImageComponent*>(mFullscreenButtonGO->GetComponent(ComponentType::IMAGE));
    image->SetAlpha(1.f);
    mCurrentSetting = SETTING_TYPE::FULL_SCREEN;

    OnGeneralVolumeHoverOff();
    OnMusicVolumeHoverOff();
    OnEffectsVolumeHoverOff();
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

void MainMenu::OnGeneralVolumeHoverOff()
{
    mGeneralVolumeFill->SetAlpha(0.8f);
}

void MainMenu::OnMusicVolumeHoverOff()
{
    mMusicVolumeFill->SetAlpha(0.8f);
}

void MainMenu::OnEffectsVolumeHoverOff()
{
    mEffectsVolumeFill->SetAlpha(0.8f);
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

