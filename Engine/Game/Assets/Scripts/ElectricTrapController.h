#pragma once
#include <Script.h>
#include <vector>
#include "Macros.h"
#include "TimerScript.h"
#include "PlayerController.h"
#include "AudioManager.h"

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
    void SetTrapSound(SFX sound) { mTrapSound = sound; }

private:

    void ActivateTrap(bool active, bool vfxOnly);

    BoxColliderComponent* mCollider = nullptr;

    float mArea = 1.0f;
    GameObject* mSfx = nullptr;

    // Activation
    bool mIsAwake = false;
    bool mIsActive = false;

    bool mFirstActivation = true;
    float mFirstActivationInterval = 1.0f;

    float mActivationInterval = 2.0f;
    float mActivationDuration = 3.0f;

    TimerScript mActivationIntervalTimer;
    TimerScript mActivationDurationTimer;

    // Damage
    float mDamageAmount = 5.0f;
    float mSpeedReduction = 0.5f;

    float mDamageTimer = 0.0f;
    float mDamageDelay = 1.0f;

    PlayerController* mPlayer = nullptr;

    SFX mTrapSound = SFX::ELECTRICAL_TRAP1;
};
