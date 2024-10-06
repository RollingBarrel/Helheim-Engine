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
class CinematicCamera;

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
    AudioManager* GetAudio() const { return mAudioManager; }
    HudController* GetHud() const { return mHudController; }
    bool UsingController() const;
    BattleArea* GetActiveBattleArea() const { return mActiveBattleArea; }
    PoolManager* GetPoolManager() const;

    bool IsPaused() const { return mPaused; }
    void SetPaused(bool value, bool screen);
    void LoadLevel(const char* LevelName);
    void SetActiveBattleArea(BattleArea* activeArea);

    void Victory();
    void GameOver();
    void HitStopTime(float time);
    void HitStop();
    void HitStop(float duration);
    bool IsStopped() const { return mStopActive; };

    void ActivateSecondTutorial();
    void UnlockSecondary();
    void UnlockUltimate(bool unlock);
    void UnlockGrenade(bool unlock);

    void SetIsFightingBoss(bool fighting) { mIsFightingBoss = fighting; }
    void PauseBackgroundAudio(bool pause);

    void HandleBossAudio(int stage);

    void RegisterPlayerKill();
    void PlayPlayerFootStepSound();
    void ActivateBossCamera(float targetDistance);
    void BossCameraMovement();

    bool IsPlayingCinematic() { return mPlayingCinematic; }

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
    GameObject* mFirstTutorial = nullptr;
    GameObject* mSecondTutorial = nullptr;
    TimerScript mHitStopTimer;
    TimerScript mLoadTimer;
    TimerScript mLoadSecondTimer;
    Timer* mGameTimer = nullptr;

    GameObject* mCinematicManagerGO = nullptr;
    CinematicCamera* mCinematicCamera = nullptr;

    bool mPlayingCinematic = false;

    bool mPaused = false;
    bool mPauseScreen = false;

    bool mStopActive = false;
    float mDefaultHitStopTime = 0.0f;
    float mHitStopTime = 0.0f;
    float mStopStart = 0.0f;
    float mCurrentStopTime = 0.0f;

    bool mController = false;

    int mBackgroundAudioID = -1;

    int mGameOverAudio = -1;
    int mLastAudioID = -1;

    bool mLoadLevel = false;
    const char* mLevelName = nullptr;

    bool mIsFightingBoss = false;
    bool mCameraLerp = false;
    float mBossCameraTarget = 11.0f;
};