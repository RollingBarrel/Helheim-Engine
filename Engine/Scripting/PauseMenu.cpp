#include "PauseMenu.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "ModuleAudio.h"
#include "ScriptComponent.h"
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
#include "GameManager.h"
#include "PlayerStats.h"

CREATE(PauseMenu)
{
    CLASS(owner);

    SEPARATOR("MENUS");
    MEMBER(MemberType::GAMEOBJECT, mMainMenu);
    MEMBER(MemberType::GAMEOBJECT, mOptionsMenu);
    MEMBER(MemberType::GAMEOBJECT, mCreditsMenu);
    MEMBER(MemberType::GAMEOBJECT, mCreditsListText);
    MEMBER(MemberType::GAMEOBJECT, mOptionsContainerGO);

    SEPARATOR("SETTINGS");
    MEMBER(MemberType::GAMEOBJECT, mGeneralVolumeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mGeneralVolumeFillGO);
    MEMBER(MemberType::GAMEOBJECT, mGeneralVolumeBackgroundGO);
    MEMBER(MemberType::GAMEOBJECT, mMusicVolumeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mMusicVolumeFillGO);
    MEMBER(MemberType::GAMEOBJECT, mMusicVolumeBackgroundGO);
    MEMBER(MemberType::GAMEOBJECT, mEffectsVolumeSliderGO);
    MEMBER(MemberType::GAMEOBJECT, mEffectsVolumeFillGO);
    MEMBER(MemberType::GAMEOBJECT, mEffectsVolumeBackgroundGO);
    MEMBER(MemberType::GAMEOBJECT, mVSyncButtonOffGO);
    MEMBER(MemberType::GAMEOBJECT, mVSyncButtonOnGO);
    MEMBER(MemberType::GAMEOBJECT, mVSyncImageOn_On);
    MEMBER(MemberType::GAMEOBJECT, mVSyncImageOff_On);
    MEMBER(MemberType::GAMEOBJECT, mVSyncBackgroundGO);
    MEMBER(MemberType::GAMEOBJECT, mFullscreenButtonOnGO);
    MEMBER(MemberType::GAMEOBJECT, mFullscreenButtonOffGO);
    MEMBER(MemberType::GAMEOBJECT, mFullscreenImageOn_On);
    MEMBER(MemberType::GAMEOBJECT, mFullscreenImageOff_On);
	MEMBER(MemberType::GAMEOBJECT, mFullscreenBackgroundGO);


    SEPARATOR("Play Btn");
    MEMBER(MemberType::GAMEOBJECT, mPlayGO);
    MEMBER(MemberType::GAMEOBJECT, mPlayText);
    MEMBER(MemberType::GAMEOBJECT, mPlayHover);
    MEMBER(MemberType::GAMEOBJECT, mPlayClicked);

    SEPARATOR("Options Btn");
    MEMBER(MemberType::GAMEOBJECT, mOptionsGO);
    MEMBER(MemberType::GAMEOBJECT, mOptionsText);
    MEMBER(MemberType::GAMEOBJECT, mOptionsHover);
    MEMBER(MemberType::GAMEOBJECT, mOptionsClicked);

    SEPARATOR("Credits Btn");
    MEMBER(MemberType::GAMEOBJECT, mCreditsGO);
    MEMBER(MemberType::GAMEOBJECT, mCreditsText);
    MEMBER(MemberType::GAMEOBJECT, mCreditsHover);
    MEMBER(MemberType::GAMEOBJECT, mCreditsClicked);

    SEPARATOR("Quit Btn");
    MEMBER(MemberType::GAMEOBJECT, mQuitGO);
    MEMBER(MemberType::GAMEOBJECT, mQuitText);
    MEMBER(MemberType::GAMEOBJECT, mQuitHover);
    MEMBER(MemberType::GAMEOBJECT, mQuitClicked);

    SEPARATOR("Back Btn");
    MEMBER(MemberType::GAMEOBJECT, mBackGO);

    SEPARATOR("Keyboard Btn");
    MEMBER(MemberType::GAMEOBJECT, mKeyboardGO);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardText);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardHover);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardClicked);

    SEPARATOR("Controller Btn");
    MEMBER(MemberType::GAMEOBJECT, mControllerGO);
    MEMBER(MemberType::GAMEOBJECT, mControllerText);
    MEMBER(MemberType::GAMEOBJECT, mControllerHover);
    MEMBER(MemberType::GAMEOBJECT, mControllerClicked);

    SEPARATOR("Audio Btn");
    MEMBER(MemberType::GAMEOBJECT, mAudioGO);
    MEMBER(MemberType::GAMEOBJECT, mAudioText);
    MEMBER(MemberType::GAMEOBJECT, mAudioHover);
    MEMBER(MemberType::GAMEOBJECT, mAudioClicked);

    SEPARATOR("Settings Btn");
    MEMBER(MemberType::GAMEOBJECT, mSettingsGO);
    MEMBER(MemberType::GAMEOBJECT, mSettingsText);
    MEMBER(MemberType::GAMEOBJECT, mSettingsHover);
    MEMBER(MemberType::GAMEOBJECT, mSettingsClicked);

    SEPARATOR("Options Screens");
    MEMBER(MemberType::GAMEOBJECT, mControlsScreenGO);
    MEMBER(MemberType::GAMEOBJECT, mSettingsScreenGO);
    MEMBER(MemberType::GAMEOBJECT, mAudioScreenGO);
    MEMBER(MemberType::GAMEOBJECT, mKeyboardScreenGO);

    SEPARATOR("Audio buttons");
    MEMBER(MemberType::GAMEOBJECT, mGeneralVolumeUp);
    MEMBER(MemberType::GAMEOBJECT, mGeneralVolumeDown);
    MEMBER(MemberType::GAMEOBJECT, mMusicVolumeUp);
    MEMBER(MemberType::GAMEOBJECT, mMusicVolumeDown);
    MEMBER(MemberType::GAMEOBJECT, mEffectsVolumeUp);
    MEMBER(MemberType::GAMEOBJECT, mEffectsVolumeDown);

    END_CREATE;
}

PauseMenu::PauseMenu(GameObject* owner) : Script(owner) {}

void PauseMenu::Start()
{
    // Main buttons

    mPlayButton = static_cast<ButtonComponent*>(mPlayGO->GetComponent(ComponentType::BUTTON));
    mPlayButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnPlayButtonClick, this)));
    mPlayButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&PauseMenu::OnPlayButtonHover, this)));
    mPlayButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&PauseMenu::OnPlayButtonHoverOff, this)));

    mOptionsButton = static_cast<ButtonComponent*>(mOptionsGO->GetComponent(ComponentType::BUTTON));
    mOptionsButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnOptionsButtonClick, this)));
    mOptionsButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&PauseMenu::OnOptionsButtonHover, this)));
    mOptionsButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&PauseMenu::OnOptionsButtonHoverOff, this)));

    mCreditsButton = static_cast<ButtonComponent*>(mCreditsGO->GetComponent(ComponentType::BUTTON));
    mCreditsButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnCreditsButtonClick, this)));
    mCreditsButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&PauseMenu::OnCreditsButtonHover, this)));
    mCreditsButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&PauseMenu::OnCreditsButtonHoverOff, this)));

    mQuitButton = static_cast<ButtonComponent*>(mQuitGO->GetComponent(ComponentType::BUTTON));
    mQuitButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnQuitButtonClick, this)));
    mQuitButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&PauseMenu::OnQuitButtonHover, this)));
    mQuitButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&PauseMenu::OnQuitButtonHoverOff, this)));

    mBackButton = static_cast<ButtonComponent*>(mBackGO->GetComponent(ComponentType::BUTTON));
    mBackButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnBackButtonClick, this)));

    // Options buttons

    mSettingsButton = static_cast<ButtonComponent*>(mSettingsGO->GetComponent(ComponentType::BUTTON));
    mSettingsButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnSettingsButtonClick, this)));
    mSettingsButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&PauseMenu::OnVideoSettingsButtonHover, this)));
    mSettingsButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&PauseMenu::OnVideoSettingsButtonHoverOff, this)));

    mControllerButton = static_cast<ButtonComponent*>(mControllerGO->GetComponent(ComponentType::BUTTON));
    mControllerButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnControllerButtonClick, this)));
    mControllerButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&PauseMenu::OnControllerButtonHover, this)));
    mControllerButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&PauseMenu::OnControllerButtonHoverOff, this)));

    mAudioButton = static_cast<ButtonComponent*>(mAudioGO->GetComponent(ComponentType::BUTTON));
    mAudioButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnAudioButtonClick, this)));
    mAudioButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&PauseMenu::OnAudioSettingsButtonHover, this)));
    mAudioButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&PauseMenu::OnAudioSettingsButtonHoverOff, this)));

    mKeyboardButton = static_cast<ButtonComponent*>(mKeyboardGO->GetComponent(ComponentType::BUTTON));
    mKeyboardButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnKeyboardButtonClick, this)));
    mKeyboardButton->AddEventHandler(EventType::HOVER, new std::function<void()>(std::bind(&PauseMenu::OnKeyboardButtonHover, this)));
    mKeyboardButton->AddEventHandler(EventType::HOVEROFF, new std::function<void()>(std::bind(&PauseMenu::OnKeyboardButtonHoverOff, this)));

    // Others

    mTextTransform = static_cast<Transform2DComponent*>(mCreditsListText->GetComponent(ComponentType::TRANSFORM2D));

    static_cast<ButtonComponent*>(mGeneralVolumeUp->GetComponent(ComponentType::BUTTON))
        ->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnGeneralUp, this)));
    static_cast<ButtonComponent*>(mGeneralVolumeDown->GetComponent(ComponentType::BUTTON))
        ->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnGeneralDown, this)));
    static_cast<ButtonComponent*>(mMusicVolumeUp->GetComponent(ComponentType::BUTTON))
        ->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnMusicUp, this)));
    static_cast<ButtonComponent*>(mMusicVolumeDown->GetComponent(ComponentType::BUTTON))
        ->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnMusicDown, this)));
    static_cast<ButtonComponent*>(mEffectsVolumeUp->GetComponent(ComponentType::BUTTON))
        ->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnEffectsUp, this)));
    static_cast<ButtonComponent*>(mEffectsVolumeDown->GetComponent(ComponentType::BUTTON))
        ->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnEffectsDown, this)));

    mGeneralVolumeSlider = static_cast<SliderComponent*>(mGeneralVolumeSliderGO->GetComponent(ComponentType::SLIDER));
    mGeneralVolumeFill = static_cast<ImageComponent*>(mGeneralVolumeFillGO->GetComponent(ComponentType::IMAGE));

    mMusicVolumeSlider = static_cast<SliderComponent*>(mMusicVolumeSliderGO->GetComponent(ComponentType::SLIDER));
    mMusicVolumeFill = static_cast<ImageComponent*>(mMusicVolumeFillGO->GetComponent(ComponentType::IMAGE));

    mEffectsVolumeSlider = static_cast<SliderComponent*>(mEffectsVolumeSliderGO->GetComponent(ComponentType::SLIDER));
    mEffectsVolumeFill = static_cast<ImageComponent*>(mEffectsVolumeFillGO->GetComponent(ComponentType::IMAGE));

    mVSyncOnButton = static_cast<ButtonComponent*>(mVSyncButtonOnGO->GetComponent(ComponentType::BUTTON));
    mVSyncOffButton = static_cast<ButtonComponent*>(mVSyncButtonOffGO->GetComponent(ComponentType::BUTTON));

    mFullscreenOnButton = static_cast<ButtonComponent*>(mFullscreenButtonOnGO->GetComponent(ComponentType::BUTTON));
    mFullscreenOffButton = static_cast<ButtonComponent*>(mFullscreenButtonOffGO->GetComponent(ComponentType::BUTTON));

    mVSyncOnButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnVSyncButtonOnClick, this)));
    mVSyncOffButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnVSyncButtonOffClick, this)));
    mFullscreenOnButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnFullscreenButtonOnClick, this)));
    mFullscreenOffButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&PauseMenu::OnFullscreenButtonOffClick, this)));

    mAudioManager = GameManager::GetInstance()->GetAudio();

    // Init the volume setting sliders
    mMasterVolumeValue = App->GetAudio()->GetVolume("bus:/");
    mMusicVolumeValue = App->GetAudio()->GetVolume("bus:/music");
    mEffectsVolumeValue = App->GetAudio()->GetVolume("bus:/sfx");
    App->GetAudio()->SetVolume("bus:/", mMasterVolumeValue);
    mGeneralVolumeSlider->SetValue(mMasterVolumeValue);
    App->GetAudio()->SetVolume("bus:/music", mMusicVolumeValue);
    mMusicVolumeSlider->SetValue(mMusicVolumeValue);
    App->GetAudio()->SetVolume("bus:/sfx", mEffectsVolumeValue);
    mEffectsVolumeSlider->SetValue(mEffectsVolumeValue);

    OpenMenu(MENU_TYPE::MAIN);
    OnPlayButtonHover(); // Hover first option when the menu is first laoded    
    OnKeyboardButtonHover(); // Pre-hover the first option

    // Init the volume sliders alpha
    mGeneralVolumeFill->SetAlpha(0.5f);
    mMusicVolumeFill->SetAlpha(0.5f);
    mEffectsVolumeFill->SetAlpha(0.5f);

}

void PauseMenu::Update()
{
    Controls();

    if (mIsScrolling)
    {
        float3 currentPosition = mTextTransform->GetPosition();
        if (currentPosition.y > 3800.0f) mTextTransform->SetPosition(float3(currentPosition.x, -700.0f, currentPosition.z));
        else mTextTransform->SetPosition(float3(currentPosition.x, currentPosition.y + 100 * App->GetDt(), currentPosition.z));
    }
    mTimePassed += App->GetDt();
}

void PauseMenu::Reset()
{
    OpenMenu(MENU_TYPE::MAIN);

    mPlayClicked->SetEnabled(false);
    mOptionsClicked->SetEnabled(false);
    mCreditsClicked->SetEnabled(false);
    mQuitClicked->SetEnabled(false);

    mControllerClicked->SetEnabled(false);
    mKeyboardClicked->SetEnabled(false);
    mAudioClicked->SetEnabled(false);
    mSettingsClicked->SetEnabled(false);

    OnPlayButtonHover(); // Hover first option when the menu is first laoded    
    OnKeyboardButtonHover(); // Pre-hover the first option
}

void PauseMenu::Controls()
{
    if (App->GetInput()->GetKey(Keys::Keys_UP) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP) == ButtonState::BUTTON_DOWN || ((App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) < -0.9f && mTimePassed >= mDebounceTime)))
    {
        if (mIsAdjustingAudio) return;
        mTimePassed = 0.0f; // Restart debounce timer
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
        if (mCurrentMenu == MENU_TYPE::MAIN) // MENU MAIN BUTTONS
        {
            if (mMainOption > 0)
            {
                mMainOption--;
            }
            else
            {
                mMainOption = 3;
            }
            HoverMenu(static_cast<MENU_TYPE>(mMainOption));
        }
        else if (mCurrentMenu == MENU_TYPE::OPTIONS)
        {


            if (mOptionsOption > 4)
            {
                mOptionsOption--;
            }
            else
            {
                mOptionsOption = 7;
            }
            HoverMenu(static_cast<MENU_TYPE>(mOptionsOption));
        }
        else if (mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS)
        {
            if (mAudioSettingOption > 0)
            {
                mAudioSettingOption--;
            }
            else
            {
                mAudioSettingOption = 2;
            }
            HoverAudioMenu(static_cast<AUDIO_SETTING_TYPE>(mAudioSettingOption));
        }
        else if (mCurrentMenu == MENU_TYPE::VIDEO_SETTINGS)
        {
            if (mVideoSettingOption > 0)
            {
                mVideoSettingOption--;
            }
            else
            {
                mVideoSettingOption = 1;
            }
            HoverVideoMenu(static_cast<VIDEO_SETTING_TYPE>(mVideoSettingOption));
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_DOWN) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN) == ButtonState::BUTTON_DOWN || ((App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) > 0.9f && mTimePassed >= mDebounceTime)))
    {
        if (mIsAdjustingAudio) return;
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
        mTimePassed = 0.0f; // Restart debounce timer
        if (mCurrentMenu == MENU_TYPE::MAIN) // MENU MAIN BUTTONS
        {
            if (mMainOption < 3)
            {
                mMainOption++;
            }
            else
            {
                mMainOption = 0;
            }
            HoverMenu(static_cast<MENU_TYPE>(mMainOption));
        }
        else if (mCurrentMenu == MENU_TYPE::OPTIONS)
        {
            if (mOptionsOption < 7)

            {
                mOptionsOption++;
            }
            else
            {
                mOptionsOption = 4;
            }
            HoverMenu(static_cast<MENU_TYPE>(mOptionsOption));
        }

        else if (mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS)
        {
            if (mAudioSettingOption < 2)
            {
                mAudioSettingOption++;
            }
            else
            {
                mAudioSettingOption = 0;
            }
            HoverAudioMenu(static_cast<AUDIO_SETTING_TYPE>(mAudioSettingOption));
        }
        else if (mCurrentMenu == MENU_TYPE::VIDEO_SETTINGS)
        {
            if (mVideoSettingOption < 1)
            {
                mVideoSettingOption++;
            }
            else
            {
                mVideoSettingOption = 0;
            }
            HoverVideoMenu(static_cast<VIDEO_SETTING_TYPE>(mVideoSettingOption));
        }
    }

	if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN ||
		App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN ||
		App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
	{
		if (mCurrentMenu == MENU_TYPE::MAIN)
		{
			mAudioManager->PlayOneShot(SFX::MAINMENU_OK);

			ClickMenu(static_cast<MENU_TYPE>(mMainOption));
		}
		else if (mCurrentMenu == MENU_TYPE::OPTIONS)
		{
			mAudioManager->PlayOneShot(SFX::MAINMENU_OK);

			OpenMenu(static_cast<MENU_TYPE>(mOptionsOption));
		}
		else if (mCurrentMenu == MENU_TYPE::VIDEO_SETTINGS)
		{
			if (mCurrentVideoSetting == VIDEO_SETTING_TYPE::VSYNC)
			{
				if (App->GetCurrentClock()->GetVsyncStatus())
				{
					OnVSyncButtonOffClick();
				}
				else
				{
					OnVSyncButtonOnClick();
				}
			}
			else if (mCurrentVideoSetting == VIDEO_SETTING_TYPE::FULL_SCREEN)
			{
				if (App->GetWindow()->IsWindowFullscreen())
				{
					OnFullscreenButtonOffClick();
				}
				else
				{
					OnFullscreenButtonOnClick();
				}
			}
		}
		else if (mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS)
		{
			mIsAdjustingAudio = true;
			if (mCurrentAudioSetting == AUDIO_SETTING_TYPE::MASTER)
			{
				mGeneralVolumeFill->SetAlpha(1.f);
			}
			else if (mCurrentAudioSetting == AUDIO_SETTING_TYPE::MUSIC)
			{
				mMusicVolumeFill->SetAlpha(1.f);
			}
			else if (mCurrentAudioSetting == AUDIO_SETTING_TYPE::EFFECTS)
			{
				mEffectsVolumeFill->SetAlpha(1.f);
			}
		}

    }

    if (App->GetInput()->GetKey(Keys::Keys_LEFT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT) == ButtonState::BUTTON_DOWN || ((App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) < -0.9f && mTimePassed >= mDebounceTime)))
    {
        mTimePassed = 0.0f; // Restart debounce timer

        if (mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS && mIsAdjustingAudio)
        {
            OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(mAudioSettingOption), DIRECTION::LEFT, 0.01f);
        }
        else if (mCurrentMenu == MENU_TYPE::VIDEO_SETTINGS || mCurrentMenu == MENU_TYPE::CONTROLS || mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS || mCurrentMenu == MENU_TYPE::KEYBOARD)
        {
            if (mCurrentMenu == MENU_TYPE::KEYBOARD)
            {
                mOptionsOption = 4;
                mKeyboardClicked->SetEnabled(false);
                OnKeyboardButtonHover();
            }
            else if (mCurrentMenu == MENU_TYPE::CONTROLS)
            {
                mOptionsOption = 5;
                mControllerClicked->SetEnabled(false);
                OnControllerButtonHover();
            }
            else if (mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS)
            {
                mAudioSettingOption = AUDIO_SETTING_TYPE::MASTER; // Reset the current setting to the first one
                mOptionsOption = 6;
                mAudioClicked->SetEnabled(false);
                OnAudioSettingsButtonHover();
            }
            else if (mCurrentMenu == MENU_TYPE::VIDEO_SETTINGS)
            {
                mVideoSettingOption = VIDEO_SETTING_TYPE::VSYNC; // Reset the current setting to the first one
                mOptionsOption = 7;
                mSettingsClicked->SetEnabled(false);
                OnVideoSettingsButtonHover();
            }
            OpenMenu(MENU_TYPE::OPTIONS);
        }
        else
        {
            mOptionsClicked->SetEnabled(false);
            mCreditsClicked->SetEnabled(false);
            OpenMenu(MENU_TYPE::MAIN);
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_RIGHT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT) == ButtonState::BUTTON_DOWN || ((App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) > 0.9f && mTimePassed >= mDebounceTime)))
    {
        mTimePassed = 0.0f; // Restart debounce timer

        if (mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS && mIsAdjustingAudio)
        {
            OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(mAudioSettingOption), DIRECTION::RIGHT, 0.01f);
        }
        else if (mCurrentMenu == MENU_TYPE::MAIN)
        {
            mAudioManager->PlayOneShot(SFX::MAINMENU_OK);

            ClickMenu(static_cast<MENU_TYPE>(mMainOption));
        }
        else if (mCurrentMenu == MENU_TYPE::OPTIONS)
        {
            mAudioManager->PlayOneShot(SFX::MAINMENU_OK);

            OpenMenu(static_cast<MENU_TYPE>(mOptionsOption));
        }
        else if (mCurrentMenu == MENU_TYPE::VIDEO_SETTINGS)
        {
            if (mCurrentVideoSetting == VIDEO_SETTING_TYPE::VSYNC)
            {
                if (App->GetCurrentClock()->GetVsyncStatus())
                {
                    OnVSyncButtonOffClick();
                }
                else
                {
                    OnVSyncButtonOnClick();
                }
            }
            else if (mCurrentVideoSetting == VIDEO_SETTING_TYPE::FULL_SCREEN)
            {
                if (App->GetWindow()->IsWindowFullscreen())
                {
                    OnFullscreenButtonOffClick();
                }
                else
                {
                    OnFullscreenButtonOnClick();
                }
            }
        }
    }

    if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_DOWN)
    {
        if (mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS)
            OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(mAudioSettingOption), DIRECTION::RIGHT, 0.1f);
    }

    if (App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == ButtonState::BUTTON_DOWN)
    {
        if (mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS)
            OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(mAudioSettingOption), DIRECTION::LEFT, 0.1f);
    }

    if (App->GetInput()->GetKey(Keys::Keys_BACKSPACE) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_DOWN)
    {
        if (mCurrentMenu == MENU_TYPE::MAIN && GameManager::GetInstance()->IsPaused())
        {
            GameManager::GetInstance()->SetPaused(false, true);
            return;
        }

        mAudioManager->PlayOneShot(SFX::MAINMENU_CANCEL);
        if (mCurrentMenu == MENU_TYPE::VIDEO_SETTINGS || mCurrentMenu == MENU_TYPE::CONTROLS || mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS || mCurrentMenu == MENU_TYPE::KEYBOARD)
        {
            if (mCurrentMenu == MENU_TYPE::KEYBOARD)
            {
                mOptionsOption = 4;
                mKeyboardClicked->SetEnabled(false);
                OnKeyboardButtonHover();
            }
            else if (mCurrentMenu == MENU_TYPE::CONTROLS)
            {
                mOptionsOption = 5;
                mControllerClicked->SetEnabled(false);
                OnControllerButtonHover();
            }
            else if (mCurrentMenu == MENU_TYPE::AUDIO_SETTINGS)
            {
                if (mIsAdjustingAudio)
                {
                    // TODO: Unselect the current volume slider 
                    mIsAdjustingAudio = false;
                    if (mCurrentAudioSetting == AUDIO_SETTING_TYPE::MASTER)
                    {
                        mGeneralVolumeFill->SetAlpha(0.5f);
                    }
                    else if (mCurrentAudioSetting == AUDIO_SETTING_TYPE::MUSIC)
                    {
                        mMusicVolumeFill->SetAlpha(0.5f);
                    }
                    else if (mCurrentAudioSetting == AUDIO_SETTING_TYPE::EFFECTS)
                    {
                        mEffectsVolumeFill->SetAlpha(0.5f);
                    }
                    return; // Early return to avoid OpenMenu() call
                }
                else
                {
                    mAudioSettingOption = AUDIO_SETTING_TYPE::MASTER; // Reset the current setting to the first one
                    mOptionsOption = 6;
                    mAudioClicked->SetEnabled(false);
                    OnAudioSettingsButtonHover();
                }
            }
            else if (mCurrentMenu == MENU_TYPE::VIDEO_SETTINGS)
            {
                mVideoSettingOption = VIDEO_SETTING_TYPE::VSYNC; // Reset the current setting to the first one
                mOptionsOption = 7;
                mSettingsClicked->SetEnabled(false);
                OnVideoSettingsButtonHover();
            }
            OpenMenu(MENU_TYPE::OPTIONS);
        }
        else
        {
            mOptionsClicked->SetEnabled(false);
            mCreditsClicked->SetEnabled(false);
            OpenMenu(MENU_TYPE::MAIN);
        }
    }
}

// MENUS
void PauseMenu::OpenMenu(MENU_TYPE type)
{
    mCurrentMenu = type;

    mOptionsMenu->SetEnabled(false);
    mCreditsMenu->SetEnabled(false);

    mSettingsScreenGO->SetEnabled(false);
    mControlsScreenGO->SetEnabled(false);
    mOptionsContainerGO->SetEnabled(false);
    mAudioScreenGO->SetEnabled(false);
    mKeyboardScreenGO->SetEnabled(false);

    //mIsScrolling = false;

    switch (type)
    {
    case MENU_TYPE::MAIN:
        mMainMenu->SetEnabled(true);
        break;
    case MENU_TYPE::OPTIONS:
        mOptionsMenu->SetEnabled(true);
        mOptionsContainerGO->SetEnabled(true);
        mOptionsClicked->SetEnabled(true);
        mCreditsClicked->SetEnabled(false);
        break;
    case MENU_TYPE::CREDITS:
        mCreditsMenu->SetEnabled(true);
        mIsScrolling = true;
        mTextTransform->SetPosition(float3(mTextTransform->GetPosition().x, 0, mTextTransform->GetPosition().z));
        mOptionsClicked->SetEnabled(false);
        mCreditsClicked->SetEnabled(true);
        break;
    case MENU_TYPE::CONTROLS:
        mOptionsMenu->SetEnabled(true);
        mOptionsContainerGO->SetEnabled(true);
        mControlsScreenGO->SetEnabled(true);
        mControllerClicked->SetEnabled(true);
        mAudioClicked->SetEnabled(false);
        mKeyboardClicked->SetEnabled(false);
        mSettingsClicked->SetEnabled(false);
        break;
    case MENU_TYPE::AUDIO_SETTINGS:
        mOptionsMenu->SetEnabled(true);
        mOptionsContainerGO->SetEnabled(true);
        mAudioScreenGO->SetEnabled(true);
        mControllerClicked->SetEnabled(false);
        mAudioClicked->SetEnabled(true);
        mKeyboardClicked->SetEnabled(false);
        mSettingsClicked->SetEnabled(false);
        OnGeneralVolumeHover(); // Hover the first button
        break;
    case MENU_TYPE::KEYBOARD:
        mOptionsMenu->SetEnabled(true);
        mOptionsContainerGO->SetEnabled(true);
        mKeyboardScreenGO->SetEnabled(true);
        mControllerClicked->SetEnabled(false);
        mAudioClicked->SetEnabled(false);
        mKeyboardClicked->SetEnabled(true);
        mSettingsClicked->SetEnabled(false);
        break;
    case MENU_TYPE::VIDEO_SETTINGS:
        mOptionsMenu->SetEnabled(true);
        mOptionsContainerGO->SetEnabled(true);
        mSettingsScreenGO->SetEnabled(true);
        mControllerClicked->SetEnabled(false);
        mAudioClicked->SetEnabled(false);
        mKeyboardClicked->SetEnabled(false);
        mSettingsClicked->SetEnabled(true);
        OnVSyncButtonHover(); // Hover the first button
        break;
    }
}

// CLICKS

void PauseMenu::ClickMenu(MENU_TYPE type)
{
    switch (type) 
    {
        case MENU_TYPE::MAIN:
            OnPlayButtonClick();
            break;
        case MENU_TYPE::OPTIONS:
            OnOptionsButtonClick();
            break;
        case MENU_TYPE::CREDITS:
            OnCreditsButtonClick();
            break;
        case MENU_TYPE::QUIT:
            OnQuitButtonClick();
            break;
    }
}

void PauseMenu::OnMainButtonClick()
{
    OpenMenu(MENU_TYPE::MAIN);
}

void PauseMenu::OnQuitButtonClick() 
{
    App->GetScene()->GetPlayerStats()->ResetStats();
    GameManager::GetInstance()->LoadLevel("Assets/Scenes/MainMenu");
}

void PauseMenu::OnOptionsButtonClick()
{
    OpenMenu(MENU_TYPE::OPTIONS);
}

void PauseMenu::OnCreditsButtonClick()
{
    OpenMenu(MENU_TYPE::CREDITS);
}

void PauseMenu::OnBackButtonClick()
{
    mOptionsClicked->SetEnabled(false);
    mCreditsClicked->SetEnabled(false);
    OpenMenu(MENU_TYPE::MAIN);
}

void PauseMenu::OnPlayButtonClick()
{
    GameManager::GetInstance()->SetPaused(false, true);
}

void PauseMenu::OnControllerButtonClick()
{
    OpenMenu(MENU_TYPE::CONTROLS);
}

void PauseMenu::OnAudioButtonClick()
{
    OpenMenu(MENU_TYPE::AUDIO_SETTINGS);
}

void PauseMenu::OnKeyboardButtonClick()
{
    OpenMenu(MENU_TYPE::KEYBOARD);
}

void PauseMenu::OnSettingsButtonClick()
{
    OpenMenu(MENU_TYPE::VIDEO_SETTINGS);
    if (App->GetWindow()->IsWindowFullscreen())
    {
        mFullscreenImageOff_On->SetEnabled(false);
        mFullscreenImageOn_On->SetEnabled(true);
    }
    else
    {
        mFullscreenImageOff_On->SetEnabled(true);
        mFullscreenImageOn_On->SetEnabled(false);
    }
}

void PauseMenu::OnVSyncButtonOnClick()
{
    mVSyncImageOff_On->SetEnabled(false);
    mVSyncImageOn_On->SetEnabled(true);

    App->GetCurrentClock()->SetVsyncStatus(true);
}

void PauseMenu::OnVSyncButtonOffClick()
{
    mVSyncImageOff_On->SetEnabled(true);
    mVSyncImageOn_On->SetEnabled(false);

    App->GetCurrentClock()->SetVsyncStatus(false);
}

void PauseMenu::OnFullscreenButtonOnClick()
{
    mFullscreenImageOff_On->SetEnabled(false);
    mFullscreenImageOn_On->SetEnabled(true);

    App->GetWindow()->WindowFullscreen(true);
}

void PauseMenu::OnFullscreenButtonOffClick()
{
    mFullscreenImageOff_On->SetEnabled(true);
    mFullscreenImageOn_On->SetEnabled(false);

    App->GetWindow()->WindowFullscreen(false);
}

void PauseMenu::OnVolumeSlide(AUDIO_SETTING_TYPE type, DIRECTION direction, float step)
{
    // Early return. Just double checking we're not calling it on elements that are not sliders within the settings menu.
    if (type != AUDIO_SETTING_TYPE::MASTER && type != AUDIO_SETTING_TYPE::MUSIC && type != AUDIO_SETTING_TYPE::EFFECTS) return;

    switch (type)
    {
    case AUDIO_SETTING_TYPE::MASTER:
        if (direction == DIRECTION::LEFT)
        {
            mMasterVolumeValue = App->GetAudio()->GetVolume("bus:/");
            if (mMasterVolumeValue - step <= 0.0f)
            {
                App->GetAudio()->SetVolume("bus:/", 0.f);
                mGeneralVolumeSlider->SetValue(0.001f);
            }
            else
            {
                App->GetAudio()->SetVolume("bus:/", mMasterVolumeValue - step);
                mGeneralVolumeSlider->SetValue(mMasterVolumeValue - step);
            }
        }
        else // DIRECTION::RIGHT
        {
            mMasterVolumeValue = App->GetAudio()->GetVolume("bus:/");
            if (mMasterVolumeValue + step > 1.f)
            {
                App->GetAudio()->SetVolume("bus:/", 1.f);
                mGeneralVolumeSlider->SetValue(1.f);
            }
            else
            {
                App->GetAudio()->SetVolume("bus:/", mMasterVolumeValue + step);
                mGeneralVolumeSlider->SetValue(mMasterVolumeValue + step);
            }
        }
        break;
    case AUDIO_SETTING_TYPE::MUSIC:
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
    case AUDIO_SETTING_TYPE::EFFECTS:
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
void PauseMenu::HoverMenu(MENU_TYPE type)
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
    case MENU_TYPE::QUIT:
        OnQuitButtonHover();
        break;
    case MENU_TYPE::CONTROLS:
        OnControllerButtonHover();
        break;
    case MENU_TYPE::VIDEO_SETTINGS:
        OnVideoSettingsButtonHover();
        break;
    case MENU_TYPE::AUDIO_SETTINGS:
        OnAudioSettingsButtonHover();
        break;
    case MENU_TYPE::KEYBOARD:
        OnKeyboardButtonHover();
        break;
    }
}

void PauseMenu::HoverAudioMenu(AUDIO_SETTING_TYPE type)
{
    switch (type)
    {
    case AUDIO_SETTING_TYPE::MASTER:
        OnGeneralVolumeHover();
        break;
    case AUDIO_SETTING_TYPE::MUSIC:
        OnMusicVolumeHover();
        break;
    case AUDIO_SETTING_TYPE::EFFECTS:
        OnEffectsVolumeHover();
        break;
    }
}

//TODO: Add Hover Effects (Waiting for the new UI)
void PauseMenu::HoverVideoMenu(VIDEO_SETTING_TYPE type)
{
    switch (type)
    {
    case VIDEO_SETTING_TYPE::VSYNC:
        OnVSyncButtonHover();
        break;
    case VIDEO_SETTING_TYPE::FULL_SCREEN:
        OnFullscreenButtonHover();
        break;
    }
}

void PauseMenu::OnGeneralVolumeHover()
{
    //TODO: Use when the slider is active - mGeneralVolumeFill->SetAlpha(1.f);
    mGeneralVolumeBackgroundGO->SetEnabled(true);
    mCurrentAudioSetting = AUDIO_SETTING_TYPE::MASTER;

    //TODO: Abstract this abomination (in all of the hover functions)
    OnMusicVolumeHoverOff();
    OnEffectsVolumeHoverOff();
}

void PauseMenu::OnMusicVolumeHover()
{
    mMusicVolumeBackgroundGO->SetEnabled(true);
    mCurrentAudioSetting = AUDIO_SETTING_TYPE::MUSIC;

    OnGeneralVolumeHoverOff();
    OnEffectsVolumeHoverOff();
}

void PauseMenu::OnEffectsVolumeHover()
{
    mEffectsVolumeBackgroundGO->SetEnabled(true);
    mCurrentAudioSetting = AUDIO_SETTING_TYPE::EFFECTS;

    OnGeneralVolumeHoverOff();
    OnMusicVolumeHoverOff();
}

void PauseMenu::OnVSyncButtonHover()
{

    mCurrentVideoSetting = VIDEO_SETTING_TYPE::VSYNC;
    mVSyncBackgroundGO->SetEnabled(true);

    OnGeneralVolumeHoverOff();
    OnMusicVolumeHoverOff();
    OnEffectsVolumeHoverOff();
    OnFullscreenButtonHoverOff();
}

void PauseMenu::OnFullscreenButtonHover()
{
    mCurrentVideoSetting = VIDEO_SETTING_TYPE::FULL_SCREEN;
    mFullscreenBackgroundGO->SetEnabled(true);

    OnGeneralVolumeHoverOff();
    OnMusicVolumeHoverOff();
    OnEffectsVolumeHoverOff();
    OnVSyncButtonHoverOff();
}
void PauseMenu::OnQuitButtonHover()
{
    if (mMainOption != 3)
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
    }

    mQuitHover->SetEnabled(true);
    mQuitText->SetEnabled(false);

    mMainOption = 3;

    // Set the other hovers off (integration mouse/click)
    OnCreditsButtonHoverOff();
    OnPlayButtonHoverOff();
    OnOptionsButtonHoverOff();
}

void PauseMenu::OnQuitButtonHoverOff()
{
    mQuitHover->SetEnabled(false);
    mQuitText->SetEnabled(true);
}

void PauseMenu::OnOptionsButtonHover()
{
    if (mMainOption != 1)
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
    }

    mOptionsHover->SetEnabled(true);
    mOptionsText->SetEnabled(false);

    mMainOption = 1;

    // Set the other hovers off (integration mouse/click)
    OnCreditsButtonHoverOff();
    OnPlayButtonHoverOff();
    OnQuitButtonHoverOff();
}

void PauseMenu::OnOptionsButtonHoverOff()
{
    mOptionsHover->SetEnabled(false);
    mOptionsText->SetEnabled(true);
}

void PauseMenu::OnCreditsButtonHover()
{
    if (mMainOption != 2)
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
    }

    mCreditsHover->SetEnabled(true);
    mCreditsText->SetEnabled(false);

    mMainOption = 2;

    // Set the other hovers off (integration mouse/click)
    OnPlayButtonHoverOff();
    OnOptionsButtonHoverOff();
    OnQuitButtonHoverOff();
}

void PauseMenu::OnCreditsButtonHoverOff()
{
    mCreditsHover->SetEnabled(false);
    mCreditsText->SetEnabled(true);
}

void PauseMenu::OnPlayButtonHover()
{
    if (mMainOption != 0)
    {
        mAudioManager->PlayOneShot(SFX::MAINMENU_SELECT);
    }

    mPlayHover->SetEnabled(true);
    mPlayText->SetEnabled(false);

    mMainOption = 0;

    // Set the other hovers off (integration mouse/click)
    OnCreditsButtonHoverOff();
    OnOptionsButtonHoverOff();
    OnQuitButtonHoverOff();
}

void PauseMenu::OnPlayButtonHoverOff()
{
    mPlayHover->SetEnabled(false);
    mPlayText->SetEnabled(true);
}

void PauseMenu::OnControllerButtonHover()
{
    //ImageComponent* image = static_cast<ImageComponent*>(mControllerGO->GetComponent(ComponentType::IMAGE));
    //image->SetAlpha(0.8f);

    mControllerHover->SetEnabled(true);
    mControllerText->SetEnabled(false);

    // Set the other hovers off (integration mouse/click)
    OnVideoSettingsButtonHoverOff();
    OnAudioSettingsButtonHoverOff();
    OnKeyboardButtonHoverOff();
}

void PauseMenu::OnControllerButtonHoverOff()
{
    //ImageComponent* image = static_cast<ImageComponent*>(mControllerGO->GetComponent(ComponentType::IMAGE));
    //image->SetAlpha(0.2f);
    mControllerHover->SetEnabled(false);
    mControllerText->SetEnabled(true);
}

void PauseMenu::OnAudioSettingsButtonHover()
{
    //ImageComponent* image = static_cast<ImageComponent*>(mAudioGO->GetComponent(ComponentType::IMAGE));
    //image->SetAlpha(0.8f);

    mAudioHover->SetEnabled(true);
    mAudioText->SetEnabled(false);

    // Set the other hovers off (integration mouse/click)
    OnVideoSettingsButtonHoverOff();
    OnControllerButtonHoverOff();
    OnKeyboardButtonHoverOff();
}

void PauseMenu::OnAudioSettingsButtonHoverOff()
{
    mAudioHover->SetEnabled(false);
    mAudioText->SetEnabled(true);

    //ImageComponent* image = static_cast<ImageComponent*>(mAudioGO->GetComponent(ComponentType::IMAGE));
    //image->SetAlpha(0.2f);
}

void PauseMenu::OnKeyboardButtonHover()
{
    //ImageComponent* image = static_cast<ImageComponent*>(mKeyboardGO->GetComponent(ComponentType::IMAGE));
    //image->SetAlpha(0.8f);

    mKeyboardHover->SetEnabled(true);
    mKeyboardText->SetEnabled(false);

    // Set the other hovers off (integration mouse/click)
    OnVideoSettingsButtonHoverOff();
    OnControllerButtonHoverOff();
    OnAudioSettingsButtonHoverOff();
}

void PauseMenu::OnKeyboardButtonHoverOff()
{
    //ImageComponent* image = static_cast<ImageComponent*>(mKeyboardGO->GetComponent(ComponentType::IMAGE));
    //image->SetAlpha(0.2f);

    mKeyboardHover->SetEnabled(false);
    mKeyboardText->SetEnabled(true);
}

void PauseMenu::OnVideoSettingsButtonHover()
{
    //ImageComponent* image = static_cast<ImageComponent*>(mSettingsButtonGO->GetComponent(ComponentType::IMAGE));
    //image->SetAlpha(0.8f);

    mSettingsHover->SetEnabled(true);
    mSettingsText->SetEnabled(false);

    // Set the other hovers off (integration mouse/click)
    OnKeyboardButtonHoverOff();
    OnControllerButtonHoverOff();
    OnAudioSettingsButtonHoverOff();
}

void PauseMenu::OnVideoSettingsButtonHoverOff()
{
    //ImageComponent* image = static_cast<ImageComponent*>(mSettingsButtonGO->GetComponent(ComponentType::IMAGE));
    //image->SetAlpha(0.2f);

    mSettingsHover->SetEnabled(false);
    mSettingsText->SetEnabled(true);
}

void PauseMenu::OnGeneralVolumeHoverOff()
{
    mGeneralVolumeBackgroundGO->SetEnabled(false);
}

void PauseMenu::OnMusicVolumeHoverOff()
{
    mMusicVolumeBackgroundGO->SetEnabled(false);
}

void PauseMenu::OnEffectsVolumeHoverOff()
{
    mEffectsVolumeBackgroundGO->SetEnabled(false);
}

void PauseMenu::OnVSyncButtonHoverOff()
{
    mVSyncBackgroundGO->SetEnabled(false);
}

void PauseMenu::OnFullscreenButtonHoverOff()
{
    mFullscreenBackgroundGO->SetEnabled(false);

}

void PauseMenu::OnGeneralUp()
{
    OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(0), DIRECTION::RIGHT, 0.1f);
}

void PauseMenu::OnGeneralDown()
{
    OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(0), DIRECTION::LEFT, 0.1f);
}

void PauseMenu::OnMusicUp()
{
    OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(1), DIRECTION::RIGHT, 0.1f);
}

void PauseMenu::OnMusicDown()
{
    OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(1), DIRECTION::LEFT, 0.1f);
}

void PauseMenu::OnEffectsUp()
{
    OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(2), DIRECTION::RIGHT, 0.1f);
}

void PauseMenu::OnEffectsDown()
{
    OnVolumeSlide(static_cast<AUDIO_SETTING_TYPE>(2), DIRECTION::LEFT, 0.1f);
}
