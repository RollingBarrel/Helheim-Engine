#pragma once
#include "Script.h"
#include "Macros.h"
#include "GameObject.h"

GENERATE_BODY(ItemShield);

class ItemShield : public Script
{
    FRIEND(ItemShield)

public:
    ItemShield(GameObject* owner);
    ~ItemShield() {}
    void Start() override;
    void Update() override;
    
private:
    bool IsPlayerInRange(float range);

    float mActivationRange = 1.0f;
    float mHealthRecovered = 15.0f;
    GameObject* mPlayer = nullptr;
};
