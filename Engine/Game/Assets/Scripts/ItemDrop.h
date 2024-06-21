#pragma once
#include "Script.h"
#include "Macros.h"
#include "GameObject.h"

GENERATE_BODY(ItemDrop);

class AnimationComponent;
class BoxColliderComponent;
struct CollisionData;
enum class EnergyType;

class ItemDrop : public Script
{
    FRIEND(ItemDrop)

public:
    ItemDrop(GameObject* owner);
    ~ItemDrop() {}
    void Start() override;
    void Update() override;
    void OnCollisionEnter(CollisionData* collisionData);

private:
    int mDropId = -1;
    float mActivationRange = 1.0f;
    float mHealthRecovered = 15.0f;
    GameObject* mPlayer = nullptr;
    AnimationComponent* mAnimation = nullptr;
    BoxColliderComponent* mCollider = nullptr;

    
};
