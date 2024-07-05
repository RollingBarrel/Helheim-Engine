#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"

class AnimationComponent;
class AnimationStateMachine;
class AudioSourceComponent;
struct CollisionData;
class BoxColliderComponent;
class Component;

class State;
class DashState;
class IdleState;
class MoveState;
class AimState;
class AttackState;
class GrenadeState;
class SwitchState;
class SpecialState;
class ReloadState;
enum StateType;

class Weapon;
class RangeWeapon;
class MeleeWeapon;
class Grenade;

enum class BattleSituation 
{
    IDLE_HIGHT_HP,
    IDLE_LOW_HP,
    BATTLE_HIGHT_HP,
    BATTLE_LOW_HP,
    DEATH
};

enum class EnergyType 
{
    NONE,
    BLUE,
    RED
};

GENERATE_BODY(PlayerController);
class PlayerController :public Script
{
    FRIEND(PlayerController)
public:
    explicit PlayerController(GameObject* owner);
    PlayerController(const PlayerController& playerController) = delete;
    ~PlayerController();
    PlayerController& operator=(const PlayerController& other) = delete;

    void Start() override;
    void Update() override;

    float3 GetPlayerDirection() { return mPlayerDirection; }
    float3 GetCollisionDirection() { return mCollisionDirection; }
    float3 GetPlayerAimPosition() { return mAimPosition; }
    float3 GetPlayerPosition();
   
    void SetAnimation(std::string trigger, float transitionTime);
    void SetSpineAnimation(std::string trigger, float transitionTime);
    void SetAnimationSpeed(float speed);

    void MoveToPosition(float3 position);
    void MoveInDirection(float3 direction);

    void SwitchWeapon();

    float GetDashCoolDown() const { return mDashCoolDown; }
    float GetDashDuration() const { return mDashDuration; }
    float GetDashRange() const { return mDashRange; }
    float GetGrenadeCooldown() const { return mGrenadeCoolDown; }
    float GetGrenadeRange() const { return mGrenadeRange;  }
    float GetSwitchCooldown() const { return mSwitchCoolDown; }
    float GetSwitchDuration() const { return mSwitchDuration; }
    float GetReloadDuration() const { return mReloadDuration; }
    int GetShieldPercetage() const { return static_cast<int>(mShield / mMaxShield) * 100.0f;}

    void EquipMeleeWeapon(bool equip);
    void EquipRangedWeapons(bool equip);
    Weapon* GetWeapon() const { return mWeapon; }
    Weapon* GetSpecialWeapon() const { return mSpecialWeapon; }
    int GetCurrentEnergy() const { return mCurrentEnergy; }
    EnergyType GetEnergyType() const { return mEnergyType; }

    void SetMovementSpeed(float percentage) { mPlayerSpeed *= percentage; }
    void SetWeaponDamage(float percentage); 
    void SetMaxShield(float percentage); 

    State* GetPlayerLowerState() const { return mLowerState; }
    State* GetPlayerUpperState() const { return mUpperState; }

    void SetSpecialWeapon(Weapon* weapon) { mSpecialWeapon = weapon; }
    void SetDashCoolDown(float value) { mDashCoolDown = value; }
    void SetDashDuration(float value) { mDashDuration = value; }
    void SetDashRange(float value) { mDashRange = value; }

    // Grenade
    void SetGrenadeCooldown(float value) { mGrenadeCoolDown = value; }
    void SetGrenadeRange(float value) { mGrenadeRange = value; }
    void SetGrenadeVisuals(bool value);
    void UpdateGrenadeVisuals();
    void ThrowGrenade();

    bool CanReload() const;
    void Reload() const;
    
    void RechargeShield(float shield);
    void TakeDamage(float damage);

    void RechargeBattery(EnergyType batteryType);
    void UseEnergy(int energy);

    // States
    DashState* GetDashState() { return mDashState; }
    IdleState* GetIdleState() { return mIdleState; }
    MoveState* GetMoveState() { return mMoveState; }
    AimState* GetAimState() { return mAimState; }
    AttackState* GetAttackState() { return mAttackState; }
    GrenadeState* GetGrenadeState() { return mGrenadeState; }
    SwitchState* GetSwitchState() { return mSwitchState; }
    SpecialState* GetSpecialState() { return mSpecialState; }
    ReloadState* GetReloadState() { return mReloadState; }

private:
    void CheckInput();
    void StateMachine();
    void HandleRotation();
    void CheckDebugOptions();
    void OnCollisionEnter(CollisionData* collisionData);

    // STATES
    State* mLowerState = nullptr;
    StateType mLowerStateType;

    DashState* mDashState = nullptr;
    IdleState* mIdleState = nullptr;
    MoveState* mMoveState = nullptr;

    State* mUpperState = nullptr;
    StateType mUpperStateType;

    AimState* mAimState = nullptr;
    AttackState* mAttackState = nullptr;
    GrenadeState* mGrenadeState = nullptr;
    SwitchState* mSwitchState = nullptr;
    SpecialState* mSpecialState = nullptr;
    ReloadState* mReloadState = nullptr;

    // MOUSE
    float3 mPlayerDirection;
    float3 mAimPosition;

    // ANIMATION
    AnimationComponent* mAnimationComponent = nullptr;
    AnimationStateMachine* mStateMachine = nullptr;

    // STATS
    // Dash
    float mDashCoolDown = 2.0f;
    float mDashDuration = 0.5f;
    float mDashRange = 5.0f;
    // Speed
    float mPlayerSpeed = 2.0f;
    // Shield
    float mShield = 100.0f;
    float mMaxShield = 100.0f;

    // WEAPONS
    Weapon* mWeapon = nullptr;
    Weapon* mSpecialWeapon = nullptr;
    int mCurrentEnergy = 0;
    EnergyType mEnergyType = EnergyType::NONE;

    // RANGED
    RangeWeapon* mPistol = nullptr;
    RangeWeapon* mMachinegun = nullptr;
    RangeWeapon* mShootgun = nullptr;

    // MELEE
    MeleeWeapon* mBat = nullptr;
    MeleeWeapon* mKatana = nullptr;
    MeleeWeapon* mHammer = nullptr;

    GameObject* mEquippedMeleeGO = nullptr;
    GameObject* mUnEquippedMeleeGO = nullptr;
    GameObject* mEquippedGunGO = nullptr;
    GameObject* mUnEquippedGunGO = nullptr;
    GameObject* mEquippedSpecialGO = nullptr;
    GameObject* mUnEquippedSpecialGO = nullptr;

    GameObject* mMeleeCollider = nullptr;
    GameObject* mBatTrail = nullptr;
    GameObject* mKatanaTrail = nullptr;
    GameObject* mHammerTrail = nullptr;

    // Attack
    float mSwitchCoolDown = 0.2f;
    float mSwitchDuration = 0.2f;
    float mReloadDuration = 0.5;

    // Grenade
    float mGrenadeCoolDown = 5.0f;
    float mGrenadeRange = 5.0f;
    float3 mGrenadePosition;
    Grenade* mGrenade = nullptr;
    GameObject* mGrenadeGO = nullptr;
    GameObject* mGrenadeExplotionPreviewAreaGO = nullptr;
    
    // Collider
    BoxColliderComponent* mCollider = nullptr;
    float3 mCollisionDirection = float3::zero;

    // Camera
    GameObject* mCamera = nullptr;

    // Debug
    bool mGodMode = false;


    //Hit Effect
    bool mHit = false;
    float mTimePassed = 0.0f;
    std::vector<Component*> mMeshComponents;
    std::vector<unsigned int> mMaterialIds;
    bool Delay(float delay);
 
};