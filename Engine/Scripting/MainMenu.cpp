#include "pch.h"
#include "MainMenu.h"

#include "GameObject.h"

CREATE(MainMenu)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::BOOL, mActiveMenu);
    END_CREATE;
}

MainMenu::MainMenu(GameObject* owner) : Script(owner)
{
    //LOG("MENU_CONSTRUCTOR");

    mActiveMenu = true;
    mPauseMenu = false;
}

void MainMenu::Update()
{
    //LOG("MAIN_UPDATE");
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
    //LOG("MAIN_STATUS");
    if (mActiveMenu) {
        //LOG("MENU");

        mGameObject->SetEnabled(mActiveMenu);
        //mGameObject->GetName().

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

}
