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

    GameObject* mLookTarget = nullptr;
    GameObject* mFollowTarget = nullptr;
    CameraComponent* mCameraComponent = nullptr;
    float3 mCameraPosition = float3(-11.90, -9.1f, 2.6f);
    float3 mCameraRotation = float3(0.60f, -1.30f, 0.0f);
    void SetView(float3 position, float3 rotation);


};

