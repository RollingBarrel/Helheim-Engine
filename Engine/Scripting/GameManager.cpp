#include "pch.h"
#include "GameManager.h"

#include "MainMenu.h"

#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Keys.h"

CREATE(GameManager)
{
    CLASS(owner);
    SEPARATOR("STATS");
    MEMBER(MemberType::BOOL, mSplashScreens);
    MEMBER(MemberType::FLOAT, mTimeScreen);
    END_CREATE;
}

GameManager::GameManager(GameObject* owner) : Script(owner) {}

void GameManager::Start() {}

void GameManager::Update()
{
    if (mSplashScreens)
    {
        Splash_Screens();
    }
    else
    {
        //SetMenu(bool active, bool pause);
    }

    Controls();
}

void GameManager::ChangeImage(const char* imageName, bool enabled) const {
    App->GetScene()->Find(imageName)->SetEnabled(enabled);
}

void GameManager::ResetScreen(const char* screenName, bool activeMenu) {
    //mMenuActive = activeMenu;
    //ChangeImage(mPreviousImageName, false);
    //ChangeImage(screenName, false);
}

void GameManager::Splash_Screens() 
{
    Engine_Name();
    Studio_Name();
    Game_Name();
}

void GameManager::Engine_Name() 
{
    /*
    ChangeImage("Engine_Name", true);

    if (Delay(mTimeScreen))
    {
        ChangeImage("Engine_Name", false);
        mStudioName = true;
    }
    */
}

void GameManager::Studio_Name()
{
    /*
    if (mStudioName) {
        ChangeImage("Studio_Name", true);
        mStudioName = false;

        if (Delay(mTimeScreen))
        {
            ChangeImage("Studio_Name", false);
            mGameName = true;
        }
    }
    */
}

void GameManager::Game_Name() 
{
    /*
    if (Game_Name) {
        ChangeImage("Game_Name", true);
        mGameName = false;

        if (Delay(mTimeScreen))
        {
            ChangeImage("Game_Name", false);
        }
    }
    */
}

void GameManager::Controls()
{
    /*
    if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
    {  
    }
    */
}
