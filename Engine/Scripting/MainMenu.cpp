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

    mTextTransform = static_cast<Transform2DComponent*>(mCreditsText->GetComponent(ComponentType::TRANSFORM2D));

    mMusicVolumeButton = static_cast<ButtonComponent*>(mMusicVolumeButtonGO->GetComponent(ComponentType::BUTTON));
    mGeneralVolumeButton = static_cast<ButtonComponent*>(mGeneralVolumeButtonGO->GetComponent(ComponentType::BUTTON));
    mEffectsVolumeButton = static_cast<ButtonComponent*>(mEffectsVolumeButtonGO->GetComponent(ComponentType::BUTTON));
    mVSyncButton = static_cast<ButtonComponent*>(mVSyncButtonGO->GetComponent(ComponentType::BUTTON));
    mFullscreenButton = static_cast<ButtonComponent*>(mFullscreenButtonGO->GetComponent(ComponentType::BUTTON));
    mVSyncImage = static_cast<ImageComponent*>(mVSyncButtonGO->GetComponent(ComponentType::IMAGE));
    mFullscreenImage = static_cast<ImageComponent*>(mFullscreenButtonGO->GetComponent(ComponentType::IMAGE));

    mVSyncButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnVSyncButtonClick, this)));
    mFullscreenButton->AddEventHandler(EventType::CLICK, new std::function<void()>(std::bind(&MainMenu::OnFullscreenButtonClick, this)));

    OpenMenu(MENU_TYPE::STUDIO);
}

void MainMenu::Update()
{
    if (mIsInitial) 
    {
        if (Delay(2.0f) && mCurrentMenu == MENU_TYPE::STUDIO) OpenMenu(MENU_TYPE::ENGINE);
        else if (Delay(2.0f) && mCurrentMenu == MENU_TYPE::ENGINE)
        {
            OpenMenu(MENU_TYPE::SPLASH);
            mIsInitial = false;
        }
        return;
    }

    if (mIsScrolling)
    {
        float3 currentPosition = mTextTransform->GetPosition();
        mTextTransform->SetPosition(float3(currentPosition.x, currentPosition.y + 100 * App->GetDt(), currentPosition.z));
    }

    if (mLoadlevel == true && Delay(0.1f)) 
    {
        App->GetScene()->Load("Assets/Scenes/Level1Scene");
    }

    Controls();
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
        //mMainMenuManager->PlaySelectSFX();
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

    if (App->GetInput()->GetKey(Keys::Keys_DOWN) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN) == ButtonState::BUTTON_DOWN)
    {
        //mMainMenuManager->PlaySelectSFX();
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

    if (App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN || 
        App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_A) == ButtonState::BUTTON_DOWN)
    {
        ClickMenu(static_cast<MENU_TYPE>(mOption));
    }

    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_B) == ButtonState::BUTTON_DOWN)
    {
        OpenMenu(static_cast<MENU_TYPE>(0));
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
            mSettingsGO->SetEnabled(true);
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
    //mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::MAIN);
}

void MainMenu::OnQuitButtonClick() {
    App->Exit();
    //mMainMenuManager->PlayOKSFX();
    exit(0);
}

void MainMenu::OnOptionsButtonClick() 
{
    //mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::OPTIONS);
}

void MainMenu::OnCreditsButtonClick() 
{
    //mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::CREDITS);
}

void MainMenu::OnNewButtonClick() 
{
    //mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::LOADING);
    mLoadlevel = true;
}

void MainMenu::OnSplashButtonClick() 
{
    OnPlayButtonHover();
    //mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::MAIN);
}

void MainMenu::OnControlsButtonClick()
{
    //mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::CONTROLS);
}

void MainMenu::OnSettingsButtonClick()
{
    //mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::SETTINGS);
}

void MainMenu::OnVSyncButtonClick()
{
    if (*mVSyncImage->GetAlpha() == 1.0f) mVSyncImage->SetAlpha(0.5f);
    else mVSyncImage->SetAlpha(1.0f);
}

void MainMenu::OnFullscreenButtonClick()
{
    if (*mFullscreenImage->GetAlpha() == 1.0f) mFullscreenImage->SetAlpha(0.5f);
    else mFullscreenImage->SetAlpha(1.0f);
}

// SELECTED

void MainMenu::HoverMenu(MENU_TYPE type) 
{

    //OnNewButtonHoverOff();
    //OnOptionsButtonHoverOff();
    //OnCreditsButtonHoverOff();
    //OnQuitButtonHoverOff();

    switch (type) {
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


void MainMenu::OnQuitButtonHover() 
{
    if (mOption != 3) 
    {
        //mMainMenuManager->PlaySelectSFX();
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
        //mMainMenuManager->PlaySelectSFX();
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
        //mMainMenuManager->PlaySelectSFX();
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
        //mMainMenuManager->PlaySelectSFX();
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
