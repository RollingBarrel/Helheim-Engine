#include "pch.h"
#include "GameManager.h"
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
    MEMBER(MemberType::BOOL, mMenuActive);
    MEMBER(MemberType::FLOAT, mTimeScreen);
    END_CREATE;
}

GameManager::GameManager(GameObject* owner) : Script(owner) {}

void GameManager::Start()
{
    ModuleScene* scene = App->GetScene();
    mPlayer = scene->FindGameObjectWithTag(scene->GetRoot(), scene->GetTagByName("Player")->GetID());
}

void GameManager::Update()
{
    if (mSplashScreens)
    {
        Splash_Screens();
    }
    else
    {
        if (mMenuActive)
        {
            mMenuActive = false;
            App->GetScene()->Find("Main_Menu")->SetEnabled(true);
        }
    }

    Controls();
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
            mNextScreen = false;
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
            mNextScreen = false;
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
            mNextScreen = false;
            mGameName = false;

            App->GetScene()->Find("Game_Name")->SetEnabled(false);

            mSplashScreens = false;
            mMenuActive = true;
        }
    }
}

bool GameManager::Delay(float delay)
{
    mTimePassed += App->GetDt();

    if (mTimePassed >= delay)
    {
        mTimePassed = 0;
        return true;
    }
    else return false;
}

void GameManager::Controls()
{
    if (!mPlayer)
    {
        if (App->GetInput()->GetKeyboardReciveInputs())
        {
            if (!mKeyPressed)
            {
                mKeyPressed = true;
                mNextScreen = true;
            }
        }
        else {
            mKeyPressed = false;
        }
    }
    else
    {
        if (App->GetInput()->GetKey(Keys::Keys_ESCAPE) == KeyState::KEY_DOWN)
        {
            mMenuActive = true;
        }
    }
}