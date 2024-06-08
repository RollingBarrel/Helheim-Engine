#pragma once
#include <Script.h>
#include "Macros.h"

class ButtonComponent;
class HudController;
class BattleArea;
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

    GameObject* GetPlayer() const { return mPlayer; };
    HudController* GetHud() const { return mHudController; };
    BattleArea* GetActiveBattleArea() const { return mActiveBattleArea; }
    
    void LoadLevel(const char* LevelName);
    void SetActiveBattleArea(BattleArea* activeArea) { mActiveBattleArea = activeArea; }

    void Victory();
    void GameOver();

private:


    static GameManager* mInstance;
    
    GameObject* mPlayer = nullptr;
    GameObject* mHudControllerGO = nullptr;

    BattleArea* mActiveBattleArea = nullptr;
    HudController* mHudController = nullptr;

    bool mPaused = false;
};