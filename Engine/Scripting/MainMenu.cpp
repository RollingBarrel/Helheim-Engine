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
    MEMBER(MemberType::BOOL, mActiveMenu);
    END_CREATE;
}

MainMenu::MainMenu(GameObject* owner) : Script(owner) {}

void MainMenu::Update()
{
    MenuStatus();
}

void MainMenu::Start() {}

void MainMenu::SetMenu(bool active, bool pause)
{
    mActiveMenu = active;
    mPauseMenu = pause;
}

void MainMenu::MenuStatus()
{
    if (mActiveMenu) {
        ChangeImage("Menu", true);

        switch (mImage)
        {
            case 1:
                if (mPreviousImageName)
                {
                    ChangeImage(mPreviousImageName, false);
                }
                
                mActualImageName = "Button_NewGame";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);
                break;
            case 2:
                ChangeImage(mPreviousImageName, false);
                mActualImageName = "Button_Options";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);
                break;
            case 3:
                ChangeImage(mPreviousImageName, false);
                mActualImageName = "Button_Credits";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);
                break;
            case 4:
                ChangeImage(mPreviousImageName, false);
                mActualImageName = "Button_Quit";
                mPreviousImageName = mActualImageName;
                ChangeImage(mActualImageName, true);
                break;
            default:
                break;
        }
        

        /*
        if (mPauseMenu) //Si esta pausado activa el menu de pausa
        {

        }
        else
        {
            mGameObject->SetEnabled(true);
        }
        */

        Controls();
    }
}

void MainMenu::ChangeImage(const char* name, bool enabled) const{
    App->GetScene()->Find(name)->SetEnabled(enabled);
}

void MainMenu::Controls()
{
    if (App->GetInput()->GetKey(Keys::Keys_UP) == KeyState::KEY_DOWN)
    {
        if (mImage > 1) 
        {
            mImage--;
        }
        else
        {
            mImage = 4;
        }        
    }

    if (App->GetInput()->GetKey(Keys::Keys_DOWN) == KeyState::KEY_DOWN)
    {
        if (mImage < 4) 
        {
            mImage++;
        }
        else
        {
            mImage = 1;
        }
    }

    if ((App->GetInput()->GetKey(Keys::Keys_RETURN) == KeyState::KEY_DOWN) || (App->GetInput()->GetKey(Keys::Keys_KP_ENTER) == KeyState::KEY_DOWN))
    {
        LOG("ENTER");
    }
}
