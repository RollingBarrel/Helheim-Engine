#pragma once
#include "Script.h"
#include "Macros.h"
#include "GameObject.h"

GENERATE_BODY(ItemHealth);

class ItemHealth : public Script
{
    FRIEND(ItemHealth)

public:
    ItemHealth(GameObject* owner);
    ~ItemHealth() {}
    void Start() override;
    void Update() override;
    
private:
    bool IsPlayerInRange(float range);

    float mActivationRange = 1.0f;
    float mHealthRecovered = 15.0f;
    GameObject* mPlayer = nullptr;
};
