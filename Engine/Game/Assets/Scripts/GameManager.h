#pragma once
#include <Script.h>
#include "Macros.h"

class ButtonComponent;
class HudController;

GENERATE_BODY(GameManager);
class GameManager : public Script
{
    FRIEND(GameManager)
public:
    GameManager(GameObject* owner); 
    ~GameManager();

    static GameManager* GetInstance(); 

    void Awake();
    void Start();
    void Update();

    GameObject* GetPlayer() { return mPlayer; };
    HudController* GetHud() { return mHudController; };
    bool UsingController() { return mController; }

    bool IsPaused() { return mPaused; }
    void SetPaused(bool value);

    void LoadLevel(const char* LevelName);

    void Victory();
    void GameOver();

private:


    static GameManager* mInstance;
    
    GameObject* mPlayer = nullptr;
    GameObject* mHudControllerGO = nullptr;

    HudController* mHudController = nullptr;

    bool mPaused = false;

    bool mController = true;
};