#pragma once
#include <Script.h>
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
};

