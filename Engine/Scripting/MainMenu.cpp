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
    SEPARATOR("STATS");
    MEMBER(MemberType::BOOL, mMenuActive);
    MEMBER(MemberType::BOOL, mPauseMenu);
    MEMBER(MemberType::GAMEOBJECT, mMainMenuManagerHolder);
    SEPARATOR("MENUS");
    MEMBER(MemberType::GAMEOBJECT, mMainMenu);
    MEMBER(MemberType::GAMEOBJECT, mOptionsMenu);
    MEMBER(MemberType::GAMEOBJECT, mCreditsMenu);
    MEMBER(MemberType::GAMEOBJECT, mLoadingMenu);
    MEMBER(MemberType::GAMEOBJECT, mSplashScreen);
    MEMBER(MemberType::GAMEOBJECT, mContainerGO);
    MEMBER(MemberType::GAMEOBJECT, mOptionsContainerGO);
    SEPARATOR("BUTTONS");
    MEMBER(MemberType::GAMEOBJECT, mSplashGO);
    MEMBER(MemberType::GAMEOBJECT, mNewGO);
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

    mSplashButton = static_cast<ButtonComponent*>(mSplashGO->GetComponent(ComponentType::BUTTON));
    mNewButton = static_cast<ButtonComponent*>(mNewGO->GetComponent(ComponentType::BUTTON));
    mOptionsButton = static_cast<ButtonComponent*>(mOptionsGO->GetComponent(ComponentType::BUTTON));
    mCreditsButton = static_cast<ButtonComponent*>(mCreditsGO->GetComponent(ComponentType::BUTTON));
    mQuitButton = static_cast<ButtonComponent*>(mQuitGO->GetComponent(ComponentType::BUTTON));
    mBackCreditButton = static_cast<ButtonComponent*>(mBackCreditGO->GetComponent(ComponentType::BUTTON));

    mSplashButton->AddEventHandler(EventType::CLICK, std::bind(&MainMenu::OnSplashButtonClick, this));
    mNewButton->AddEventHandler(EventType::CLICK, std::bind(&MainMenu::OnNewButtonClick, this));
    mOptionsButton->AddEventHandler(EventType::CLICK, std::bind(&MainMenu::OnOptionsButtonClick, this));
    mCreditsButton->AddEventHandler(EventType::CLICK, std::bind(&MainMenu::OnCreditsButtonClick, this));
    mQuitButton->AddEventHandler(EventType::CLICK, std::bind(&MainMenu::OnQuitButtonClick, this));
    mBackCreditButton->AddEventHandler(EventType::CLICK, std::bind(&MainMenu::OnMainButtonClick, this));

    mNewButton->AddEventHandler(EventType::HOVER, std::bind(&MainMenu::OnNewButtonHover, this));
    mOptionsButton->AddEventHandler(EventType::HOVER, std::bind(&MainMenu::OnOptionsButtonHover, this));
    mCreditsButton->AddEventHandler(EventType::HOVER, std::bind(&MainMenu::OnCreditsButtonHover, this));
    mQuitButton->AddEventHandler(EventType::HOVER, std::bind(&MainMenu::OnQuitButtonHover, this));

    mNewButton->AddEventHandler(EventType::HOVEROFF, std::bind(&MainMenu::OnNewButtonHoverOff, this));
    mOptionsButton->AddEventHandler(EventType::HOVEROFF, std::bind(&MainMenu::OnOptionsButtonHoverOff, this));
    mCreditsButton->AddEventHandler(EventType::HOVEROFF, std::bind(&MainMenu::OnCreditsButtonHoverOff, this));
    mQuitButton->AddEventHandler(EventType::HOVEROFF, std::bind(&MainMenu::OnQuitButtonHoverOff, this));

    OpenMenu(MENU_TYPE::SPLASH);
}

void MainMenu::Update()
{
    if (mLoadlevel == true && Delay(0.1f)) 
    {
        App->GetScene()->Load("Level1");
    }
    
    Controls();
}

/*void MainMenu::SetMenu(bool active, bool pause)
{
    mMenuActive = active;
    mPauseMenu = pause;

    Update();
    LOG("MENU ACTIVE");
}*/

/*void MainMenu::ButtonsPosition(const char* imageName, float position) const
{
    Transform2DComponent* transformComponent = static_cast<Transform2DComponent*>(App->GetScene()->Find(imageName)->GetComponent(ComponentType::TRANSFORM2D));

    if (transformComponent)
    {
        float3 currentPosition = transformComponent->GetPosition();
        float3 newPosition(currentPosition.x, position, currentPosition.z);

        transformComponent->SetPosition(newPosition);
    }
}*/

/*void MainMenu::Menu()
{
    if (mMenuActive) 
    {
        if (!mPauseMenu) 
        {
            mScreen = "Menu";
            ChangeImage(mScreen, true);
        }
        else
        {
            mScreen = "Pause_Game";
            ChangeImage(mScreen, true);
        }
        
        switch (mOption)
        {
            case 1:
                if (mPauseMenu) 
                {
                    if (mPreviousImageName)
                    {
                        ChangeImage(mPreviousImageName, false);
                    }
                        
                    mActualImageName = "Button_ContinueGame";
                    mPreviousImageName = mActualImageName;
                    ChangeImage(mActualImageName, true);

                    if (mNextScreen) 
                    {
                        mNextScreen = false;
                        ResetScreen(mScreen, false);
                    }
                } 
                break;
            case 2:
                if (mPreviousImageName)
                {
                    ChangeImage(mPreviousImageName, false);
                }
         
                mActualImageName = "Button_NewGame";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);
                
                if (mNextScreen) 
                {
                    mNextScreen = false;
                    ResetScreen(mScreen, false);
                    mLoadingActive = true;
                    Loading();
                }
                break;
            case 3:
                ChangeImage(mPreviousImageName, false);
                mActualImageName = "Button_Options";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);
                
                if (mNextScreen) 
                {
                    mNextScreen = false;
                    ResetScreen(mScreen, false);
                    mOptionsActive = true;
                    Options();
                }
                break;
            case 4:
                ChangeImage(mPreviousImageName, false);
                mActualImageName = "Button_Credits";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);

                if (mNextScreen) 
                {
                    mNextScreen = false;
                    ResetScreen(mScreen, false);
                    mCreditsActive = true;
                    Credits();
                }
                break;
            case 5:
                ChangeImage(mPreviousImageName, false);
                mActualImageName = "Button_Quit";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);

                if (mNextScreen) 
                {
                    exit(0);
                }
                break;
            default:
                break;
        }
    }
}*/

/*void MainMenu::ChangeImage(const char* imageName, bool enabled) const
{
    App->GetScene()->Find(imageName)->SetEnabled(enabled);
}*/

/*void MainMenu::ResetScreen(const char* screenName, bool activeMenu)
{
    mMenuActive = activeMenu;
    ChangeImage(mPreviousImageName, false);
    ChangeImage(screenName, false);
}*/

/*void MainMenu::Loading()
{
    if (mLoadingActive) 
    {
        ChangeImage("Loading_Screen", true);

        if (Delay(2.0f))
        {
            delete mMainMenuManager;
            mMainMenuManager = nullptr;
            ChangeImage("Loading_Screen", false);
            App->GetScene()->Load("Level1.json");
        }
    }
}*/

/*void MainMenu::Options()
{
    if (mOptionsActive) 
    {
        ChangeImage("Options", true);

        if (mPrevScreen) 
        {
            mPrevScreen = false;
            mOptionsActive = false;
            ChangeImage(mScreen, true);
            ResetScreen("Options", true);
        }
    }
}*/

/*void MainMenu::Credits()
{
    if (mCreditsActive) 
    {
        ChangeImage("Credits", true);

        if (mPrevScreen) 
        {
            mPrevScreen = false;
            mCreditsActive = false;
            ChangeImage(mScreen, true);
            ResetScreen("Credits", true);
        }
    }
}*/

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
    }

    if ((App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN) || (App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN))
    {
        


        /*if (!mReturnPressed)
        {
            mMainMenuManager->PlayOKSFX();
            mOptionTmp = mOption;
            mReturnPressed = true;
            mEscPressed = false;
            mNextScreen = true;
            
        }*/
    }

    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
    {
        /*if (!mEscPressed)
        {
            mOption = mOptionTmp;
            mEscPressed = true;
            mReturnPressed = false;
            mPrevScreen = true;
        } */     
    }
}

// MENUS

void MainMenu::OpenMenu(MENU_TYPE type) {
    switch (type) {
        case MENU_TYPE::MAIN:
            mMainMenu->SetEnabled(true);
            mOptionsMenu->SetEnabled(false);
            mCreditsMenu->SetEnabled(false);
            mLoadingMenu->SetEnabled(false);
            mSplashScreen->SetEnabled(false);
            mContainerGO->SetEnabled(true);
            mBackCreditGO->SetEnabled(false);
            mOptionsContainerGO->SetEnabled(false);
            break;
        case MENU_TYPE::OPTIONS:
            mMainMenu->SetEnabled(false);
            mOptionsMenu->SetEnabled(true);
            mCreditsMenu->SetEnabled(false);
            mLoadingMenu->SetEnabled(false);
            mSplashScreen->SetEnabled(false);
            mContainerGO->SetEnabled(true);
            mBackCreditGO->SetEnabled(false);
            mOptionsContainerGO->SetEnabled(true);
            break;
        case MENU_TYPE::CREDITS:
            mMainMenu->SetEnabled(false);
            mOptionsMenu->SetEnabled(false);
            mCreditsMenu->SetEnabled(true);
            mLoadingMenu->SetEnabled(false);
            mSplashScreen->SetEnabled(false);
            mContainerGO->SetEnabled(false);
            mBackCreditGO->SetEnabled(true);
            mOptionsContainerGO->SetEnabled(false);
            break;
        case MENU_TYPE::LOADING:
            mMainMenu->SetEnabled(false);
            mOptionsMenu->SetEnabled(false);
            mCreditsMenu->SetEnabled(false);
            mLoadingMenu->SetEnabled(true);
            mSplashScreen->SetEnabled(false);
            mContainerGO->SetEnabled(false);
            mBackCreditGO->SetEnabled(false);
            mOptionsContainerGO->SetEnabled(false);
            break;
        case MENU_TYPE::SPLASH:
            mMainMenu->SetEnabled(false);
            mOptionsMenu->SetEnabled(false);
            mCreditsMenu->SetEnabled(false);
            mLoadingMenu->SetEnabled(false);
            mSplashScreen->SetEnabled(true);
            mContainerGO->SetEnabled(false);
            mBackCreditGO->SetEnabled(false);
            mOptionsContainerGO->SetEnabled(false);
            break;
    }
}

// CLICKS

void MainMenu::OnMainButtonClick() {
    mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::MAIN);
}

void MainMenu::OnQuitButtonClick() {
    App->Exit();
    mMainMenuManager->PlayOKSFX();
}

void MainMenu::OnOptionsButtonClick() {
    mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::OPTIONS);
}

void MainMenu::OnCreditsButtonClick() {
    mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::CREDITS);
}

void MainMenu::OnNewButtonClick() {
    mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::LOADING);
    mLoadlevel = true;
}

void MainMenu::OnSplashButtonClick() {
    mMainMenuManager->PlayOKSFX();
    OpenMenu(MENU_TYPE::MAIN);
}

// SELECTED

void MainMenu::OnQuitButtonHover() {
    if (mOption != 3) 
    {
        mMainMenuManager->PlaySelectSFX();
    }
    ImageComponent* image = static_cast<ImageComponent*>(mQuitGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(0.8f, 0.8f, 0.8f));
    mOption = 3;
}

void MainMenu::OnQuitButtonHoverOff() {
    ImageComponent* image = static_cast<ImageComponent*>(mQuitGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(1, 1, 1));
    mOption = -1;
}

void MainMenu::OnOptionsButtonHover() {
    if (mOption != 1)
    {
        mMainMenuManager->PlaySelectSFX();
    }
    ImageComponent* image = static_cast<ImageComponent*>(mOptionsGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(0.8f, 0.8f, 0.8f));
    mOption = 1;
}

void MainMenu::OnOptionsButtonHoverOff() {
    ImageComponent* image = static_cast<ImageComponent*>(mOptionsGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(1, 1, 1));
    mOption = -1;
}

void MainMenu::OnCreditsButtonHover() {
    if (mOption != 2)
    {
        mMainMenuManager->PlaySelectSFX();
    }
    ImageComponent* image = static_cast<ImageComponent*>(mCreditsGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(0.8f, 0.8f, 0.8f));
    mOption = 2;
}

void MainMenu::OnCreditsButtonHoverOff() {
    ImageComponent* image = static_cast<ImageComponent*>(mCreditsGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(1, 1, 1));
    mOption = -1;
}

void MainMenu::OnNewButtonHover() {
    if (mOption != 0)
    {
        mMainMenuManager->PlaySelectSFX();
    }
    ImageComponent* image = static_cast<ImageComponent*>(mNewGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(0.8f, 0.8f, 0.8f));
    mOption = 0;
}

void MainMenu::OnNewButtonHoverOff() {
    ImageComponent* image = static_cast<ImageComponent*>(mNewGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(1, 1, 1));
    mOption = -1;
}

void MainMenu::OnBackCreditsButtonHover() {
    ImageComponent* image = static_cast<ImageComponent*>(mBackCreditGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(0.8f, 0.8f, 0.8f));
    mOption = 0;
}

void MainMenu::OnBackCreditsButtonHoverOff() {
    ImageComponent* image = static_cast<ImageComponent*>(mBackCreditGO->GetComponent(ComponentType::IMAGE));
    image->SetColor(float3(1, 1, 1));
    mOption = -1;
}
