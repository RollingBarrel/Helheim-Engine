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
    void ActivateCamera(GameObject* cinematicCamera);
    void StartCinematic(GameObject* camera, GameObject* target, int animState);
    float3 lerp(const float3& start, const float3& end, float t);
    void InitAnimation(int animState);
    bool Fade();

    GameObject* mCinematicCamera1 = nullptr;
    GameObject* mCinematicCamera2 = nullptr;
    GameObject* mCinematicCamera3 = nullptr;
    GameObject* mCinematicCamera4 = nullptr;

    GameObject* mEnemy1 = nullptr;
    GameObject* mEnemy2 = nullptr;
    GameObject* mEnemy3 = nullptr;
    GameObject* mEnemy4 = nullptr;

    GameObject* mBattleArea1 = nullptr;
    GameObject* mBattleArea2 = nullptr;
    GameObject* mBattleArea3 = nullptr;
    GameObject* mBattleArea4 = nullptr;

    GameObject* mFade = nullptr;

    BattleArea* mBArea1 = nullptr;
    BattleArea* mBArea2 = nullptr;
    BattleArea* mBArea3 = nullptr;
    BattleArea* mBArea4 = nullptr;

    TimerScript mTimer;

    int mEnemy1_AnimState = 1;
    int mEnemy2_AnimState = 1;
    int mEnemy3_AnimState = 1;
    int mEnemy4_AnimState = 1;
    
    float mDistanceToEnemy = 4.0f;
    float mYawAngle = 45.0f;
    float mPitchAngle = -45.0f;
    float3 mTargetPosition;
    float mSpeedFactor = 1.0f;
    float mTimePassed = 0.0f;
    float mAnimationTime = 7.3f;

    float mCounter = 0.0f;

    bool mMoveCompleted = false;
    bool mPlayingCinematic = true;
    bool mCinematicStarted = false;
    bool mStartParameters = false;
    bool mLevel1 = false;

    const CameraComponent* mCurrentCamera = nullptr;
    CameraComponent* mMainCamera = nullptr;
    CameraComponent* mCinematicCamera = nullptr;  

    AnimationComponent* mAnimationComponent = nullptr;
    ImageComponent* mImage = nullptr;
};