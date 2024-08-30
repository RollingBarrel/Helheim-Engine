#pragma once
#include <Script.h>
#include "Macros.h"
#include "TimerScript.h"

class ButtonComponent;
class HudController;
class PlayerController;
class AudioManager;
class BattleArea;
class EnemyPool;
class PoolManager;
class PlayerCamera;
class Timer;

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
    PlayerCamera* GetPlayerCamera() const { return mPlayerCamera; }
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
    void HitStopTime(float time);
    void HitStop();
    void HitStop(float duration);
    bool IsStopped() { return mStopActive; };

private:
    void PrepareAudio();
    void StartAudio();
    void HandleAudio();
    void EndAudio();

    void HandleLevel1Audio();
    void HandleLevel2Audio();

    static GameManager* mInstance;
    
    GameObject* mPlayer = nullptr;
    GameObject* mPlayerCameraGO = nullptr;
    GameObject* mHudControllerGO = nullptr;
    GameObject* mAudioManagerGO = nullptr;

    PlayerController* mPlayerController = nullptr;
    PlayerCamera* mPlayerCamera = nullptr;
    BattleArea* mActiveBattleArea = nullptr;
    HudController* mHudController = nullptr;
    AudioManager* mAudioManager = nullptr;
    GameObject* mPoolManager = nullptr;
    TimerScript mHitStopTimer;
    Timer* mGameTimer = nullptr;

    bool mPaused = false;

    bool mStopActive = false;
    float mDefaultHitStopTime = 0.0f;
    float mHitStopTime = 0.0f;
    float mStopStart = 0.0f;
    float mCurrentStopTime = 0.0f;

    bool mController = false;

    int mBackgroundAudioID = -1;
    int mLastAudioID = -1;

    bool mLoadLevel = false;
    const char* mLevelName = nullptr;
};