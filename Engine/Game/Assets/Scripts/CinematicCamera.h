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
    ~CinematicCamera();

    static CinematicCamera* GetInstance();

    void Awake() override;
    void Start() override;
    void Update() override;
    bool GetPlayingCinematic() { return mPlayingCinematic; }

private:
    static CinematicCamera* mInstance;

    void StartCinematic(GameObject* dummy, bool isDummy, BattleArea* battleArea, int animState, float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
    void UpdateCinematic(GameObject* dummy, bool isDummy, BattleArea* battleArea);
    void LocateCamera(float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
    bool HandleFadeIn();
    bool HandleFadeOut(GameObject* dummy, bool isDummy);
    void HandleCameraMovement();
    void HandleEscape(GameObject* dummy, bool isDummy);
    void EndCinematic(GameObject* dummy, bool isDummy);
    void InitAnimation(int animState);
    bool Fade(bool fadeOut);
    void ActivateCamera(bool state);
    void ActivateBattleArea(BattleArea* battleArea, bool state);
    void ActivateDummy(GameObject* dummy, bool state);
 
    GameObject* mPlayerCameraGO = nullptr;
    GameObject* mCinematicCameraGO = nullptr;

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
    GameObject* mHudGO = nullptr;

    BattleArea* mBattleArea1 = nullptr;
    BattleArea* mBattleArea2 = nullptr;
    BattleArea* mBattleArea3 = nullptr;
    BattleArea* mBattleArea4 = nullptr;

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
    float mAnimationTime = 8.3f;

    float mCounter = 0.0f;
    float3 mColor = float3(0.0f, 0.0f, 0.0f);
    float mFadeSpeed = 0.05f;

    bool mIsDummy1 = true;
    bool mIsDummy2 = true;
    bool mIsDummy3 = true;
    bool mIsDummy4 = true;

    bool mMoveCompleted = false;
    bool mPlayingCinematic = false;
    bool mTravelling = false;
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