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
    void Controls();
    bool Delay(float delay);

    bool mSplashScreens = true;
    bool mEngine_Name = true;
    bool mStudioName = false;
    bool mGameName = false;
    bool mNextScreen = false;
    bool mReturnPressed = false;
    float mTimeScreen = 5.0f;
    float mTimePassed = 0.0f;
};