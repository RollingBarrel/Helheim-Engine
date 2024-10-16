#pragma once
#include <Script.h>
#include <vector>
#include "Macros.h"
#include "TimerScript.h"

struct CollisionData;
class BoxColliderComponent;
class GameObject;

GENERATE_BODY(ElectricTrapController);
class ElectricTrapController :
    public Script
{
    FRIEND(ElectricTrapController)
public:
    ElectricTrapController(GameObject* owner);
    ~ElectricTrapController();
    void Update() override;
    void Start() override;
    void OnCollisionEnter(CollisionData* collisionData);

    void SetAwake(bool awake) { mIsAwake = awake; }

private:
    bool IsInTrap(const GameObject* target);
    void ActivateTrap(bool active, bool vfxOnly);

    BoxColliderComponent* mCollider = nullptr;
    std::vector<GameObject*> mInTrap;

    float mArea = 1.0f;
    GameObject* mSfx = nullptr;

    // Activation
    bool mIsAwake = false;
    bool mIsActive = false;

    bool mFirstActivation = true;
    float mFirstActivationInterval = 1.0f;

    float mActivationInterval = 4.0f;
    float mActivationDuration = 2.0f;
    float mVFXWarningDuration = 0.5f;
    TimerScript mActivationIntervalTimer;
    TimerScript mActivationDurationTimer;
    TimerScript mVFXWarningTimer;

    // Damage
    float mDamageAmount = 5.0f;
    float mSpeedReduction = 0.5f;
};
