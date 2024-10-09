#pragma once
#include "Script.h"
#include "Macros.h"
#include "GameObject.h"
#include "TimerScript.h"

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

    void Init(); 
    void Start() override;
    void Update() override;
    void OnCollisionEnter(CollisionData* collisionData);
    void CheckDistance();

private:
    int mDropId = -1;
    float mHealthRecovered = 15.0f;
    bool mAlreadyUsed = false;
    GameObject* mPlayer = nullptr;
    AnimationComponent* mAnimation = nullptr;
    BoxColliderComponent* mCollider = nullptr;

    float mDespawnTimer = 0.0f;

    TimerScript mInteractTimer;
    float mUIDeactivateTimer = 3.0f;

    bool mCollided = false;
};
