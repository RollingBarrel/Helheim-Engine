#pragma once
#include "Script.h"
#include "Macros.h"
#include "TimerScript.h"

class ImageComponent;

GENERATE_BODY(HealVFX);

class HealVFX : public Script 
{
    FRIEND(HealVFX)

public:
    HealVFX(GameObject* owner);
    ~HealVFX() {}

    void Init();
    void Start() override;
    void Update() override;

private:
    TimerScript mTimer;
    ImageComponent* mSpriteSheet = nullptr;
};

