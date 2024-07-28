#pragma once
#include "Script.h"
#include "Macros.h"

class CameraComponent;
class BattleArea;

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
    void StartCinematic(GameObject* target, float posX, float posY, float posZ, float rotX, float rotY, float rotZ);
    float3 lerp(const float3& start, const float3& end, float t);
    bool Delay(float delay);

    GameObject* mEnemy1 = nullptr;
    GameObject* mEnemy2 = nullptr;
    GameObject* mEnemy3 = nullptr;
    GameObject* mEnemy4 = nullptr;

    GameObject* mBattleArea1 = nullptr;
    GameObject* mBattleArea2 = nullptr;
    GameObject* mBattleArea3 = nullptr;

    BattleArea* mBArea1 = nullptr;
    BattleArea* mBArea2 = nullptr;
    BattleArea* mBArea3 = nullptr;

    float mDistanceToEnemy = 4.0f;
    float mYawAngle = 45.0f;
    float mPitchAngle = -53.0f;
    float3 mTargetPosition;
    float mSpeedFactor = 1.0f;
    float mTimePassed = 0.0f;

    float mAnimationTime = 8.0f;
    bool mMoveCompleted = false;
    bool mPlayingCinematic = true;;
    bool mCinematicStarted = false;
    bool mStartParameters = false;

    const CameraComponent* mCurrentCamera = nullptr;
    CameraComponent* mMainCamera = nullptr;
    CameraComponent* mCinematicCamera = nullptr;  
};