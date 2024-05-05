#pragma once
#include <Script.h>
#include "Macros.h"

GENERATE_BODY(GameManager);
class GameManager : public Script
{
    FRIEND(GameManager)
public:
    GameManager(GameObject* owner);
    ~GameManager() {}
    void Start() override;
    void Update() override;

private:
    void Splash_Screens();
    void Engine_Name();
    void Studio_Name();
    void Game_Name();
    bool Delay(float delay);
    void Controls();

    bool mSplashScreens = false;
    bool mMenuActive = false;
    bool mEngine_Name = true;
    bool mStudioName = false;
    bool mGameName = false;
    bool mNextScreen = false;
    bool mKeyPressed = false;
    bool mLoadingActive = false;

    float mTimeScreen = 5.0f;
    float mTimePassed = 0.0f;

    GameObject* mPlayer = nullptr;
    GameObject* mMenuScript = nullptr;
};