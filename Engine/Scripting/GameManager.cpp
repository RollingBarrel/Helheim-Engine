#include "GameManager.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleInput.h"
#include "Keys.h"

#include "MainMenu.h"

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
    //App->GetScene()->Find("Game_Name")->SetEnabled(false);

    if (mSplashScreens)
    {
        //Splash_Screens();
        Controls();
    }
    else
    {
        /*
        MainMenu* menuScript = (MainMenu*)(App->GetScene()->Find("Main_Menu")->GetComponent(ComponentType::SCRIPT));

        if (menuScript)
        {
            menuScript->SetMenu(true, true);
            //menuScript->Start();
        }
        */
    } 
}


void GameManager::Splash_Screens() 
{
    Engine_Name();
    Studio_Name();
    Game_Name();
}

void GameManager::Engine_Name() 
{
    if (mEngine_Name) 
    {
        App->GetScene()->Find("Engine_Name")->SetEnabled(true);
        
        if ((Delay(mTimeScreen)) || (mNextScreen))
        {
            mEngine_Name = false;
            App->GetScene()->Find("Engine_Name")->SetEnabled(false);
            mStudioName = true;
        }
    }
}

void GameManager::Studio_Name()
{
    if (mStudioName) 
    {
        App->GetScene()->Find("Studio_Game")->SetEnabled(true);
        
        if ((Delay(mTimeScreen)) || (mNextScreen))
        {
            mStudioName = false;
            App->GetScene()->Find("Studio_Game")->SetEnabled(false);
            mGameName = true;
        }
    }
}

void GameManager::Game_Name() 
{
    if (mGameName) 
    {
        App->GetScene()->Find("Game_Name")->SetEnabled(true);
        
        if (mNextScreen)
        {
            mGameName = false;
            App->GetScene()->Find("Game_Name")->SetEnabled(false);
            mSplashScreens = false;
        }
    }
}

bool GameManager::Delay(float delay)
{
    mTimePassed += App->GetGameDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}

void GameManager::Controls()
{
    if (App->GetInput()->GetKey(Keys::Keys_N) == KeyState::KEY_DOWN)
    {  
        if (!mReturnPressed) 
        {
            mReturnPressed = true;
            mNextScreen = true;
        }
    }
    
    /*
    if (App->GetInput()->GetKey(Keys::Keys_N) == KeyState::KEY_UP)
    {
        mReturnPressed = false;
    }
    */
}

/*

void PlayerController::WinMessage() {
    LOG("YOU WIN");
}

void PlayerController::LoseMessage() {
    LOG("YOU LOSE");
}

*/