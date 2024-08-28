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
};

