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

MainMenu::MainMenu(GameObject* owner) : Script(owner)
{
    mActiveMenu = false;
    mPauseMenu = false;
}

void MainMenu::Update()
{
    MenuStatus();
}

void MainMenu::Start() 
{
    LOG("MAIN_START");
}

void MainMenu::SetMenu(bool active, bool pause)
{
    mActiveMenu = active;
    mPauseMenu = pause;
}

void MainMenu::MenuStatus()
{
    if (mActiveMenu) {
        App->GetScene()->Find("Button_NewGame")->SetEnabled(true);
        App->GetScene()->Find("Menu")->SetEnabled(true);

        /*
        switch (mScreen)
        {
            case 1:
                App->GetScene()->Find("Button_NewGame")->SetEnabled(mResetScreen);
                App->GetScene()->Find("Menu")->SetEnabled(mResetScreen);
            break;
            case 2:
                //mResetScreen = true;
                //App->GetScene()->Find("Options")->SetEnabled(mResetScreen);
            break;
        }
        
        */
        /*
        if (App->GetScene()->Find("Menu")) {
            //LOG("ENCONTRADO MENU");
        }
        */

        //mGameObject->SetEnabled(mActiveMenu);

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

void MainMenu::Controls()
{
    if (App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_DOWN)
    {
        mScreen++;
    }
}
