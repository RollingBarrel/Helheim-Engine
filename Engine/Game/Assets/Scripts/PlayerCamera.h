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
    void Start() override;
    void Update() override;




private:

    GameObject* mLookTarget = nullptr;
    GameObject* mFollowTarget = nullptr;
    CameraComponent* mCameraComponent = nullptr;
    float3 mCameraPosition = float3(0, -9.5f, 10.0f);
    float3 mCameraRotation = float3(0.65f, 0.0f, 0.0f);
    void SetView(float3 position, float3 rotation);


};

