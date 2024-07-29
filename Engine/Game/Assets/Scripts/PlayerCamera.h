#pragma once
#include "Script.h"
#include "Macros.h"

class CameraComponent;

GENERATE_BODY(PlayerCamera);
class PlayerCamera :public Script
{
    FRIEND(PlayerCamera)

public:
    PlayerCamera(GameObject* owner);
    void Awake() override;
    void Start() override;
    void Update() override;

private:
    float3 lerp(const float3& start, const float3& end, float t);

    GameObject* mFollowTarget = nullptr;
    float mDistanceToPlayer = 18.0f;
    float mYawAngle = 45.0f;
    float mPitchAngle = -53.0f;
    float3 mTargetPosition;
    float mSpeedFactor = 1.0f;
    bool mMoveCompleted = false;
};