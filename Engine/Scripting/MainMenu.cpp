#include "pch.h"
#include "MainMenu.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Keys.h"

CREATE(MainMenu)
{
    CLASS(owner);
    SEPARATOR("STATS");
    //MEMBER(MemberType::BOOL, mActiveMenu);
    END_CREATE;
}

MainMenu::MainMenu(GameObject* owner) : Script(owner) {}

void MainMenu::Update()
{
    Menu();
    Loading();
    Options();
    Credits();
    Controls();
}

void MainMenu::Start() {}

void MainMenu::SetMenu(bool active, bool pause)
{
    mMenuActive = active;
    mPauseMenu = pause;
}

void MainMenu::Menu()
{
    if (mMenuActive) {
        if (!mPauseMenu) {
            ChangeImage("Menu", true);
        }
        else
        {
            ChangeImage("Pause_Game", true);
        }
        
        switch (mOption)
        {
            case 1:
                if (mPreviousImageName)
                {
                    ChangeImage(mPreviousImageName, false);
                }

                mActualImageName = "Button_NewGame";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);
                
                if (mNextScreen) {
                    mNextScreen = false;
                    ResetScreen("Menu", false);
                    mLoadingActive = true;
                    Loading();
                }
                break;
            case 2:
                ChangeImage(mPreviousImageName, false);
                mActualImageName = "Button_Options";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);
                
                if (mNextScreen) {
                    mNextScreen = false;
                    ResetScreen("Menu", false);
                    mOptionsActive = true;
                    Options();
                }
                break;
            case 3:
                ChangeImage(mPreviousImageName, false);
                mActualImageName = "Button_Credits";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);

                if (mNextScreen) {
                    mNextScreen = false;
                    ResetScreen("Menu", false);
                    mCreditsActive = true;
                    Credits();
                }
                break;
            case 4:
                ChangeImage(mPreviousImageName, false);
                mActualImageName = "Button_Quit";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);

                if (mNextScreen) {
                    exit(0);
                }
                break;
            default:
                break;
        }
    }
}

void MainMenu::ChangeImage(const char* imageName, bool enabled) const{
    App->GetScene()->Find(imageName)->SetEnabled(enabled);
}

void MainMenu::ResetScreen(const char* screenName, bool activeMenu) {
    mMenuActive = activeMenu;
    ChangeImage(mPreviousImageName, false);
    ChangeImage(screenName, false);
}

void MainMenu::Loading() {
    if (mLoadingActive) {
        ChangeImage("Loading_Screen", true);

        if (Delay(2.0f))
        {
            ChangeImage("Loading_Screen", false);
            App->GetScene()->Load("scene.json");
        }
    }
}

void MainMenu::Options() {
    if (mOptionsActive)
    {
        ChangeImage("Options", true);

        if (mPrevScreen) {
            mPrevScreen = false;
            mOptionsActive = false;
            ChangeImage("Menu", true);
            ResetScreen("Options", true);
        }
    }
}

void MainMenu::Credits() {
    if (mCreditsActive) {
        ChangeImage("Credits", true);

        if (mPrevScreen) {
            mPrevScreen = false;
            mCreditsActive = false;
            ChangeImage("Menu", true);
            ResetScreen("Credits", true);
        }
    }
}

bool MainMenu::Delay(float delay)
{
    mTimePassed += App->GetGameDt();

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
        if (mOption > 1)
        {
            mOption--;
        }
        else
        {
            mOption = 4;
        }        
    }

    if (App->GetInput()->GetKey(Keys::Keys_DOWN) == KeyState::KEY_DOWN)
    {
        if (mOption < 4)
        {
            mOption++;
        }
        else
        {
            mOption = 1;
        }
    }

    if ((App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN) || (App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN))
    {
        mNextScreen = true;
    }

    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
    {
        mPrevScreen = true;
    }
}
