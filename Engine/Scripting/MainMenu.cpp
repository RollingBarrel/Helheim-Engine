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

CREATE(MainMenu)
{
    CLASS(owner);
    SEPARATOR("SCRIPTS");
    MEMBER(MemberType::GAMEOBJECT, mMainMenuManagerHolder);

    SEPARATOR("MENUS");
    MEMBER(MemberType::GAMEOBJECT, mSplashScreen);
    MEMBER(MemberType::GAMEOBJECT, mStudioScreen);
    MEMBER(MemberType::GAMEOBJECT, mEngineScreen);
    MEMBER(MemberType::GAMEOBJECT, mLoadingScreen);

    MEMBER(MemberType::GAMEOBJECT, mMainMenu);
    MEMBER(MemberType::GAMEOBJECT, mOptionsMenu);
    MEMBER(MemberType::GAMEOBJECT, mCreditsMenu);

    //MEMBER(MemberType::GAMEOBJECT, mContainerGO);
    //MEMBER(MemberType::GAMEOBJECT, mOptionsContainerGO);
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
    if (mMainMenuManagerHolder != nullptr) 
    {
        ScriptComponent* script = (ScriptComponent*)mMainMenuManagerHolder->GetComponent(ComponentType::SCRIPT);
        mMainMenuManager = (MainMenuManager*)script->GetScriptInstance();
    }

    mSplashButton = static_cast<ButtonComponent*>(mSplashScreen->GetComponent(ComponentType::BUTTON));
    mPlayButton = static_cast<ButtonComponent*>(mPlayGO->GetComponent(ComponentType::BUTTON));
    mOptionsButton = static_cast<ButtonComponent*>(mOptionsGO->GetComponent(ComponentType::BUTTON));
    mCreditsButton = static_cast<ButtonComponent*>(mCreditsGO->GetComponent(ComponentType::BUTTON));
    mQuitButton = static_cast<ButtonComponent*>(mQuitGO->GetComponent(ComponentType::BUTTON));
    mBackCreditButton = static_cast<ButtonComponent*>(mBackCreditGO->GetComponent(ComponentType::BUTTON));

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

    OpenMenu(MENU_TYPE::SPLASH);
}

void MainMenu::Update()
{
    if (mLoadlevel == true && Delay(0.1f)) 
    {
        App->GetScene()->Load("Assets/Scenes/Level1Scene");
    }
    
    /*int mouseWheelDelta = GetMouseWheelDelta();
    if (mouseWheelDelta != 0)
    {
        // Adjust the position based on the mouse wheel delta.
        float scrollSpeed = 0.1f; // Adjust this value as needed.
        float3 newPosition = mPosition;
        newPosition.y += mouseWheelDelta * scrollSpeed;
        newPosition.y = std::max(mLowerLimit.y, std::min(mPosition.y, mUpperLimit.y));
        SetPosition(newPosition);
    }*/

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
    if (App->GetInput()->GetKey(Keys::Keys_UP) == KeyState::KEY_DOWN)
    {
        mMainMenuManager->PlaySelectSFX();
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

    if (App->GetInput()->GetKey(Keys::Keys_DOWN) == KeyState::KEY_DOWN)
    {
        mMainMenuManager->PlaySelectSFX();
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

    if ((App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN) || (App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN))
    {
        ClickMenu(static_cast<MENU_TYPE>(mOption));
    }

    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
    {
        OpenMenu(static_cast<MENU_TYPE>(0));
    }
}

// MENUS

void MainMenu::OpenMenu(MENU_TYPE type) 
{
    mEngineScreen->SetEnabled(false);
    mStudioScreen->SetEnabled(false);
    mSplashScreen->SetEnabled(false);
    mLoadingScreen->SetEnabled(false);

    //mMainMenu->SetEnabled(false);
    mOptionsMenu->SetEnabled(false);
    mCreditsMenu->SetEnabled(false);

    //mBackCreditGO->SetEnabled(false);
    //mContainerGO->SetEnabled(false);
    //mOptionsContainerGO->SetEnabled(false);

    switch (type) 
    {
        case MENU_TYPE::MAIN:
            mMainMenu->SetEnabled(true);
            //mContainerGO->SetEnabled(true);
            break;
        case MENU_TYPE::OPTIONS:
            mOptionsMenu->SetEnabled(true);
            //mContainerGO->SetEnabled(true);
            break;
        case MENU_TYPE::CREDITS:
            mCreditsMenu->SetEnabled(true);
            //mBackCreditGO->SetEnabled(true);
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
