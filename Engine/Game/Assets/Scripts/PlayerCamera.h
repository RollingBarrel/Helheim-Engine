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
    GameObject* mAuxLookTarget = nullptr;
    CameraComponent* mCameraComponent = nullptr;

    void CameraManager();
    void SetView(float3 position, float rotation);


};

