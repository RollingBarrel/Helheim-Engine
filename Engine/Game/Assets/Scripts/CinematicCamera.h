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
    void ActivateCamera(GameObject* cinematicCamera, bool state);

    //void StartCinematic(GameObject* camera, GameObject* dummy, BattleArea* battleArea, int animState);
    void StartCinematic(GameObject* camera, GameObject* dummy, BattleArea* battleArea, int animState, float posX, float posY, float posZ, float rotX, float rotY, float rotZ);

    void UpdateCinematic(GameObject* camera, GameObject* dummy, BattleArea* battleArea);
    bool HandleFadeIn(GameObject* camera);
    void HandleCameraMovement(GameObject* camera);
    bool HandleFadeOut(GameObject* dummy, BattleArea* battleArea);
    void HandleEscape(GameObject* dummy, BattleArea* battleArea);
    void EndCinematic(GameObject* dummy, BattleArea* battleArea);
    void InitAnimation(int animState);
    bool Fade(bool fadeOut);
    void ActivateBattleArea(BattleArea* battleArea, bool state);
    void ActivateDummy(GameObject* dummy, bool state);
 
    GameObject* mPlayerCameraGO = nullptr;
    GameObject* mCinematicCameraGO1 = nullptr;
    GameObject* mCinematicCameraGO2 = nullptr;
    GameObject* mCinematicCameraGO3 = nullptr;
    GameObject* mCinematicCameraGO4 = nullptr;

    GameObject* mDummyGO1 = nullptr;
    GameObject* mDummyGO2 = nullptr;
    GameObject* mDummyGO3 = nullptr;
    GameObject* mDummyGO4 = nullptr;

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

    int mDummy1AnimState = 1;
    int mDummy2AnimState = 1;
    int mDummy3AnimState = 1;
    int mDummy4AnimState = 1;
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

    bool mMoveCompleted = false;
    bool mPlayingCinematic = true;
    bool mTravelling = true;
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