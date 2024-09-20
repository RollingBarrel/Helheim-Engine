#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

class CameraComponent;
class BattleArea;
class AnimationComponent;
class ImageComponent;

GENERATE_BODY(CinematicCamera);
class CinematicCamera :public Script
{
    FRIEND(CinematicCamera)

public:
    CinematicCamera(GameObject* owner);
    void Awake() override;
    void Start() override;
    void Update() override;

private:
    void DeActivateCameras();
    void ActivateCamera(GameObject* cinematicCamera);
    void StartCinematic(GameObject* camera, GameObject* target, int animState);
    void UpdateCinematic(GameObject* camera);
    bool HandleFadeIn(GameObject* camera);
    void HandleCameraMovement(GameObject* camera);
    bool HandleFadeOut(GameObject* camera);
    void HandleEscape();
    void EndCinematic();
    void InitAnimation(int animState);
    bool Fade(bool fadeOut);
    void ResetParameters(bool CinematicStarted);
 
    GameObject* mPlayerCameraGO = nullptr;
    GameObject* mCinematicCameraGO1 = nullptr;
    GameObject* mCinematicCameraGO2 = nullptr;
    GameObject* mCinematicCameraGO3 = nullptr;
    GameObject* mCinematicCameraGO4 = nullptr;

    GameObject* mEnemyGO1 = nullptr;
    GameObject* mEnemyGO2 = nullptr;
    GameObject* mEnemyGO3 = nullptr;
    GameObject* mEnemyGO4 = nullptr;

    GameObject* mBattleAreaGO1 = nullptr;
    GameObject* mBattleAreaGO2 = nullptr;
    GameObject* mBattleAreaGO3 = nullptr;
    GameObject* mBattleAreaGO4 = nullptr;

    GameObject* mPlayerGO = nullptr;
    GameObject* mFadeGO = nullptr;

    BattleArea* mBattleArea1 = nullptr;
    BattleArea* mBattleArea2 = nullptr;
    BattleArea* mBattleArea3 = nullptr;
    BattleArea* mBattleArea4 = nullptr;

    TimerScript mTimer;

    int mEnemy1AnimState = 1;
    int mEnemy2AnimState = 1;
    int mEnemy3AnimState = 1;
    int mEnemy4AnimState = 1;
    
    float mDistanceToEnemy = 4.0f;
    float mYawAngle = 45.0f;
    float mPitchAngle = -45.0f;
    float3 mTargetPosition;
    float mSpeedFactor = 1.0f;
    float mAnimationTime = 7.3f;

    float mCounter = 0.0f;
    float3 mColor = float3(0.0f, 0.0f, 0.0f);
    float mFadeSpeed = 0.05f;

    bool mMoveCompleted = false;
    bool mPlayingCinematic = true;
    bool mTravelling = true;
    bool mCinematicStarted = false;
    bool mStartParameters = false;
    bool mLevel1 = false;
    bool mFadeStart = false;
    bool mFadeOn = false;
    bool mEscape = false;
    bool mActiveCinematicCamera = false;

    CameraComponent* mCinematicCamera = nullptr;  
    AnimationComponent* mAnimationComponent = nullptr;
    ImageComponent* mImage = nullptr;
};