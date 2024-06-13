#pragma once
#include "Script.h"
#include "Macros.h"
#include "GameObject.h"

GENERATE_BODY(ItemDrop);

class AnimationComponent;

class ItemDrop : public Script
{
    FRIEND(ItemDrop)

public:
    ItemDrop(GameObject* owner);
    ~ItemDrop() {}
    void Start() override;
    void Update() override;
    
private:
    bool IsPlayerInRange(float range);

    float mActivationRange = 1.0f;
    float mHealthRecovered = 15.0f;
    GameObject* mPlayer = nullptr;
    AnimationComponent* mAnimation = nullptr;
};
