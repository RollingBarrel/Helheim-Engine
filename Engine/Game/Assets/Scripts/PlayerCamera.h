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

    //// Funzione per ottenere la direzione della telecamera
    //float3 GetCameraDirection() const;


private:

    GameObject* mFollowTarget = nullptr;
    float mDistanceToPlayer = 18.0f;
    float mYawAngle = 45.0f;
    float mPitchAngle = 53.0f;


};

