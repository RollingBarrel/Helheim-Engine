#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

class CameraComponent;
class BattleArea;
class AnimationComponent;
class ImageComponent;
class PlayerController;

GENERATE_BODY(CinematicCamera);
class CinematicCamera :public Script
{
    FRIEND(CinematicCamera)

public:
    CinematicCamera(GameObject* owner);
    ~CinematicCamera();

    void Awake() override;
    void Start() override;
    void Update() override;
    bool GetPlayingCinematic() { return mPlayingCinematic; }

private:
    void StartCinematic(GameObject* cameraObject, GameObject* dummy, BattleArea* battleArea, int animState);
    void UpdateCinematic(GameObject* dummy, BattleArea* battleArea);
    void LocateCamera(GameObject* cameraObject);
    bool HandleFadeIn();
    bool HandleFadeOut(GameObject* dummy);
    void HandleCameraMovement();
    void HandleEscape(GameObject* dummy);
    void EndCinematic(GameObject* dummy);
    void InitAnimation(int animState);
    bool Fade(bool fadeOut);
    void ActivateCamera(bool state);
    void ActivateBattleArea(BattleArea* battleArea, bool state);
    void ActivateDummy(GameObject* dummy, bool state);
 
    GameObject* mPlayerCameraGO = nullptr;
    GameObject* mCinematicCameraGO = nullptr;

    GameObject* mCameraObjectGO1 = nullptr;
    GameObject* mCameraObjectGO2 = nullptr;
    GameObject* mCameraObjectGO3 = nullptr;
    GameObject* mCameraObjectGO4 = nullptr;

    GameObject* mEnemyGO1 = nullptr;
    GameObject* mEnemyGO2 = nullptr;
    GameObject* mEnemyGO3 = nullptr;
    GameObject* mEnemyGO4 = nullptr;

    GameObject* mBattleAreaGO1 = nullptr;
    GameObject* mBattleAreaGO2 = nullptr;
    GameObject* mBattleAreaGO3 = nullptr;
    GameObject* mBattleAreaGO4 = nullptr;

    GameObject* mFadeGO = nullptr;
    GameObject* mHudGO = nullptr;

    BattleArea* mBattleArea1 = nullptr;
    BattleArea* mBattleArea2 = nullptr;
    BattleArea* mBattleArea3 = nullptr;
    BattleArea* mBattleArea4 = nullptr;

    GameObject* mPlayer = nullptr;
    PlayerController* mPlayerController = nullptr;
    
    TimerScript mTimer;

    int mEnemyAnimState1 = 1;
    int mEnemyAnimState2 = 1;
    int mEnemyAnimState3 = 1;
    int mEnemyAnimState4 = 1;
    int mCinematicIndex = 1;
 
    float mDistanceToEnemy = 4.0f;
    float mYawAngle = 45.0f;
    float mPitchAngle = -45.0f;
    float3 mTargetPosition;
    float mSpeedFactor = 1.0f;
    float mAnimationTime = 7.83f;

    float mCounter = 0.0f;
    float3 mColor = float3(0.0f, 0.0f, 0.0f);
    float mFadeSpeed = 0.045f;

    bool mMoveCompleted = false;
    bool mPlayingCinematic = false;
    bool mTravelling = false;
    bool mStartParameters = false;
    bool mFadeStart = false;
    bool mFadeOn = false;
    bool mEscape = false;
    bool mActiveCinematicCamera = false;

    CameraComponent* mCinematicCamera = nullptr;  
    AnimationComponent* mAnimationComponent = nullptr;
    ImageComponent* mImage = nullptr;
};