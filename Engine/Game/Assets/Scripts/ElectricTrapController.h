#pragma once
#include <Script.h>
#include <vector>
#include "Macros.h"
#include "TimerScript.h"

struct CollisionData;
class BoxColliderComponent;
class GameObject;

enum class TRAP_STATE
{
    INACTIVE,
    WARNING,
    ACTIVE
};

GENERATE_BODY(ElectricTrapController);
class ElectricTrapController : public Script
{
    FRIEND(ElectricTrapController)
public:
    ElectricTrapController(GameObject* owner);
    ~ElectricTrapController();

    void Update() override;
    void Start() override;
    void OnCollisionEnter(CollisionData* collisionData);

private:
    void SetTrapState(TRAP_STATE state);
    bool IsInTrap(const GameObject* target);
    void ActivateWarningVFX(bool active);
    void ActivateTrapVFX(bool active);
    void DealDamage(GameObject* target);

    // Components
    BoxColliderComponent* mCollider = nullptr;
    std::vector<GameObject*> mInTrap;
    GameObject* mWarningVFX = nullptr;
    GameObject* mActiveVFX = nullptr;

    // State management
    TRAP_STATE mState = TRAP_STATE::INACTIVE;
    TimerScript mWarningTimer;
    TimerScript mActiveTimer;

    // Timings
    float mWarningDuration = 1.0f;
    float mActiveDuration = 4.0f;

    // Trap properties
    float mDamageAmount = 5.0f;
    float mSpeedReduction = 0.5f; // Reduce speed by 50%
};
