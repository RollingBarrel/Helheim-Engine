#pragma once
#include <Script.h>
#include "Macros.h"

class ButtonComponent;
class HudController;
class PlayerController;
class AudioManager;
class BattleArea;
class EnemyPool;
class PoolManager;

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
    void Clean();

    GameObject* GetPlayer() const { return mPlayer; }
    PlayerController* GetPlayerController() const { return mPlayerController; }
    AudioManager* GetAudio() const { return mAudioManager;}
    HudController* GetHud() const { return mHudController; }
    bool UsingController() const;
    BattleArea* GetActiveBattleArea() const { return mActiveBattleArea; }
    PoolManager* GetPoolManager() const;

    bool IsPaused() { return mPaused; }
    void SetPaused(bool value, bool screen);

    void LoadLevel(const char* LevelName);
    void SetActiveBattleArea(BattleArea* activeArea);

    void Victory();
    void GameOver();

private:
    void PrepareAudio();
    void StartAudio();
    void HandleAudio();
    void EndAudio();

    void HandleLevel1Audio();

    static GameManager* mInstance;
    
    GameObject* mPlayer = nullptr;
    GameObject* mHudControllerGO = nullptr;
    GameObject* mAudioManagerGO = nullptr;

    PlayerController* mPlayerController = nullptr;
    BattleArea* mActiveBattleArea = nullptr;
    HudController* mHudController = nullptr;
    AudioManager* mAudioManager = nullptr;
    GameObject* mPoolManager = nullptr;


    bool mPaused = false;

    bool mController = false;

    int mBackgroundAudioID = -1;
    int mLastAudioID = -1;

    bool mLoadLevel = false;
    const char* mLevelName = nullptr;
};