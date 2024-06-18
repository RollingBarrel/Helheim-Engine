#pragma once
#include <Script.h>
#include "Macros.h"

class ButtonComponent;
class HudController;
class BattleArea;
class EnemyPool;

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
    bool UsingController() const { return mController; }
    BattleArea* GetActiveBattleArea() const { return mActiveBattleArea; }
    EnemyPool* GetEnemyPool() const { return mEnemyPool; }

    bool IsPaused() { return mPaused; }
    void SetPaused(bool value);

    void LoadLevel(const char* LevelName);
    void SetActiveBattleArea(BattleArea* activeArea) { mActiveBattleArea = activeArea; }

    void Victory();
    void GameOver();

private:


    static GameManager* mInstance;
    
    GameObject* mPlayer = nullptr;
    GameObject* mHudControllerGO = nullptr;

    BattleArea* mActiveBattleArea = nullptr;
    EnemyPool* mEnemyPool = nullptr;
    HudController* mHudController = nullptr;

    bool mPaused = false;

    bool mController = false;
};