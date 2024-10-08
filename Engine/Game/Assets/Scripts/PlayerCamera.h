#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

class CameraComponent;

GENERATE_BODY(PlayerCamera);
class PlayerCamera :public Script
{
    FRIEND(PlayerCamera)

public:
    PlayerCamera(GameObject* owner) : Script(owner) {}
    void Start() override;
    void Update() override;
    void ActivateShake(float duration, float positionOffsetStrength);

    void SetDistanceToPlayer(float dist) { mDistanceToPlayer = dist; }
    float GetDistanceToPlayer() const { return mDistanceToPlayer; }

    void SetOffset(float offset) { mOffset.x = offset; }
    float GetOffset() const { return mOffset.x; }

private:
    void Shake();
    
    
    GameObject* mFollowTarget = nullptr;
    float mYawAngle = 45.0f;
    float mPitchAngle = -45.0f;
    float mDistanceToPlayer = 11.0f;

    GameObject* mCameraObject = nullptr;

    TimerScript mShakeTimer;
    float mShakeDuration = 0.0f;
    float mShakePositionOffsetStrength = 0.0f;

    float3 mOffset = float3::zero;
};

