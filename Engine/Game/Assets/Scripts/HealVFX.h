#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

class PointLightComponent;

GENERATE_BODY(HealVFX);

class HealVFX : public Script 
{
    FRIEND(HealVFX)

public:
    HealVFX(GameObject* owner);
    ~HealVFX() {}

    void Start() override;
    void Update() override;

private:
    TimerScript mTimer;
    PointLightComponent* mPointLightTop = nullptr;
    PointLightComponent* mPointLightBot = nullptr;

    float mIntensity = 1.0f;
    float mDeactivateTime = 1.0f;
    float mFadeingTime = 1.0f;
};

