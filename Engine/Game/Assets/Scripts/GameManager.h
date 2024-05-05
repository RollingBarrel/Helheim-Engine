#pragma once
#include <Script.h>
#include "Macros.h"

class ButtonComponent;

GENERATE_BODY(GameManager);
class GameManager : public Script
{
    FRIEND(GameManager)
public:
    GameManager(GameObject* owner);
    ~GameManager() {}
    void Start() override;
    void Update() override;

    void WinScreen();
    void LoseScreen();
private:
    void Loading();
    void Controls();
    bool Delay(float delay);
    void Controls();

    void OnWinButtonClick();
    void OnLoseButtonClick();
    void OnYesButtonClick();
    void OnNoButtonClick();

    void OnYesButtonHoverOn();
    void OnNoButtonHoverOn();
    void OnYesButtonHoverOff();
    void OnNoButtonHoverOff();

    GameObject* mPauseScreen = nullptr;
    GameObject* mWinScreen = nullptr;
    GameObject* mLoseScreen = nullptr;
    GameObject* mLoadingScreen = nullptr;

    GameObject* mYesGO = nullptr;
    GameObject* mNoGO = nullptr;

    ButtonComponent* mLoseBtn = nullptr;
    ButtonComponent* mWinBtn = nullptr;
    ButtonComponent* mYesBtn = nullptr;
    ButtonComponent* mNoBtn = nullptr;

    bool mLoading = false;
    bool mPaused = false;
    float mTimeScreen = 5.0f;
    float mTimePassed = 0.0f;

    GameObject* mPlayer = nullptr;
    GameObject* mMenuScript = nullptr;
};