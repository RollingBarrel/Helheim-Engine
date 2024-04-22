#include "pch.h"
#include "MainMenu.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "Transform2DComponent.h"
#include "ButtonComponent.h"

CREATE(MainMenu)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::BOOL, mMenuActive);
    MEMBER(MemberType::BOOL, mPauseMenu);
    SEPARATOR("MENUS");
    MEMBER(MemberType::GAMEOBJECT, mMainMenu);
    MEMBER(MemberType::GAMEOBJECT, mOptionsMenu);
    MEMBER(MemberType::GAMEOBJECT, mCreditsMenu);
    MEMBER(MemberType::GAMEOBJECT, mLoadingMenu);
    SEPARATOR("BUTTONS");
    MEMBER(MemberType::GAMEOBJECT, mContinueGO);
    MEMBER(MemberType::GAMEOBJECT, mNewGO);
    MEMBER(MemberType::GAMEOBJECT, mOptionsGO);
    MEMBER(MemberType::GAMEOBJECT, mCreditsGO);
    MEMBER(MemberType::GAMEOBJECT, mQuitGO);
    END_CREATE;
}

MainMenu::MainMenu(GameObject* owner) : Script(owner) {}

void MainMenu::Start() 
{
    mContinueButton = static_cast<ButtonComponent*>(mContinueGO->GetComponent(ComponentType::BUTTON));
    mNewButton = static_cast<ButtonComponent*>(mNewGO->GetComponent(ComponentType::BUTTON));
    mOptionsButton = static_cast<ButtonComponent*>(mOptionsGO->GetComponent(ComponentType::BUTTON));
    mCreditsButton = static_cast<ButtonComponent*>(mCreditsGO->GetComponent(ComponentType::BUTTON));
    mQuitButton = static_cast<ButtonComponent*>(mQuitGO->GetComponent(ComponentType::BUTTON));

    mNewButton->AddEventHandler(EventType::Click, std::bind(&MainMenu::OnNewButtonClick, this));
    mOptionsButton->AddEventHandler(EventType::Click, std::bind(&MainMenu::OnOptionsButtonClick, this));
    mCreditsButton->AddEventHandler(EventType::Click, std::bind(&MainMenu::OnCreditsButtonClick, this));
    mQuitButton->AddEventHandler(EventType::Click, std::bind(&MainMenu::OnQuitButtonClick, this));

    OpenMenu(MENU_TYPE::MAIN);
}

void MainMenu::Update()
{
    Menu();
    Loading();
    Options();
    Credits();
    Controls();
}

void MainMenu::SetMenu(bool active, bool pause)
{
    mMenuActive = active;
    mPauseMenu = pause;

    Update();
    LOG("MENU ACTIVE");
}

void MainMenu::ButtonsPosition(const char* imageName, float position) const
{
    Transform2DComponent* transformComponent = static_cast<Transform2DComponent*>(App->GetScene()->Find(imageName)->GetComponent(ComponentType::TRANSFORM2D));

    if (transformComponent)
    {
        float3 currentPosition = transformComponent->GetPosition();
        float3 newPosition(currentPosition.x, position, currentPosition.z);

        transformComponent->SetPosition(newPosition);
    }
}

void MainMenu::Menu()
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
}

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

/*bool MainMenu::Delay(float delay)
{
    mTimePassed += App->GetGameDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}*/

void MainMenu::Controls()
{
    if (App->GetInput()->GetKey(Keys::Keys_UP) == KeyState::KEY_DOWN)
    {
        if (!mPauseMenu) 
        {
            if (mOption > 2)
            {
                mOption--;
            }
            else
            {
                mOption = 5;
            }
        }
        else
        {
            if (mOption > 1)
            {
                mOption--;
            }
            else
            {
                mOption = 5;
            }
        }         
    }

    if (App->GetInput()->GetKey(Keys::Keys_DOWN) == KeyState::KEY_DOWN)
    {
        if (!mPauseMenu) 
        {
            if (mOption < 5)
            {
                mOption++;
            }
            else
            {
                mOption = 2;
            }
        }
        else
        {
            if (mOption < 5)
            {
                mOption++;
            }
            else
            {
                mOption = 1;
            }
        }
    }

    if ((App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN) || (App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN))
    {
        if (!mReturnPressed) 
        {
            mOptionTmp = mOption;
            mReturnPressed = true;
            mEscPressed = false;
            mNextScreen = true;
        }
    }

    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
    {
        if (!mEscPressed) 
        {
            mOption = mOptionTmp;
            mEscPressed = true;
            mReturnPressed = false;
            mPrevScreen = true;
        }       
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
            break;
        case MENU_TYPE::OPTIONS:
            mMainMenu->SetEnabled(false);
            mOptionsMenu->SetEnabled(true);
            mCreditsMenu->SetEnabled(false);
            mLoadingMenu->SetEnabled(false);
            break;
        case MENU_TYPE::CREDITS:
            mMainMenu->SetEnabled(false);
            mOptionsMenu->SetEnabled(false);
            mCreditsMenu->SetEnabled(true);
            mLoadingMenu->SetEnabled(false);
            break;
        case MENU_TYPE::LOADING:
            mMainMenu->SetEnabled(false);
            mOptionsMenu->SetEnabled(false);
            mCreditsMenu->SetEnabled(false);
            mLoadingMenu->SetEnabled(true);
            break;
    }
}

void MainMenu::OnMainButtonClick() {
    OpenMenu(MENU_TYPE::MAIN);
}

void MainMenu::OnQuitButtonClick() {
    
}

void MainMenu::OnOptionsButtonClick() {
    OpenMenu(MENU_TYPE::OPTIONS);
}

void MainMenu::OnCreditsButtonClick() {
    OpenMenu(MENU_TYPE::CREDITS);
}

void MainMenu::OnNewButtonClick() {
    OpenMenu(MENU_TYPE::LOADING);
    App->GetScene()->Load("Level1.json");
}
