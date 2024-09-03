#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"
#include "float4.h"
#include "TimerScript.h"

class Component;
class AnimationComponent;
class AnimationStateMachine;
class AudioSourceComponent;
class MeshRendererComponent;
class BoxColliderComponent;
struct CollisionData;

class PlayerStats;

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
class UltimateState;
class UltimateChargeState;
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
    float GetShieldPercetage() const { return ( mShield /mMaxShield) * 100.0f;}
    float GetDamageModifier() const { return mDamageModifier; }
    GameObject* GetShootOriginGO() const { return mShootOrigin; }

    void EquipMeleeWeapon(bool equip);
    void EquipRangedWeapons(bool equip);
    Weapon* GetWeapon() const { return mWeapon; }
    Weapon* GetSpecialWeapon() const { return mSpecialWeapon; }
    int GetCurrentEnergy() const { return mCurrentEnergy; }
    EnergyType GetEnergyType() const { return mEnergyType; }

    void SetMovementSpeed(float percentage);
    void SetWeaponDamage(float percentage); 
    void SetMaxShield(float percentage); 

    State* GetPlayerLowerState() const { return mLowerState; }
    State* GetPlayerUpperState() const { return mUpperState; }

    void SetSpecialWeapon(Weapon* weapon) { mSpecialWeapon = weapon; }
    void SetDashCoolDown(float value) { mDashCoolDown = value; }
    void SetDashDuration(float value) { mDashDuration = value; }
    void SetDashRange(float value) { mDashRange = value; }
    GameObject* GetDashVFX() const { return mDashVFX; }

    // Grenade
    void SetGrenadeCooldown(float value) { mGrenadeCoolDown = value; }
    void SetGrenadeRange(float value) { mGrenadeRange = value; }
    void SetGrenadeVisuals(bool value);
    void UpdateGrenadeVisuals();
    void ThrowGrenade();

    void CheckOtherTimers();

    void Paralyzed(float percentage, bool paralysis);

    bool CanReload() const;
    void Reload() const;
    
    void RechargeShield(float shield);
    void TakeDamage(float damage);

    void RechargeBattery(EnergyType batteryType);
    void UseEnergy(int energy);


    //Hit Effect
    void ActivateHitEffect();
    
    //Ultimate
    GameObject* GetUltimateGO() const{ return mUltimateGO; };
    void AddUltimateResource();
    int GetUltimateResource() const { return mUltimateResource; };
    float GetUltimateCooldown() const { return mUltimateCooldown; };
    float GetUltimateSlow() const { return mUltimatePlayerSlow; };
    float GetUltimateDuration() const { return mUltimateDuration; };
    float GetUltimateChargeDuration() const { return mUltimateChargeDuration; }
    float GetUltimateDamageInterval() const { return mUltimateDamageInterval; };
    float GetUltimateDamageTick() const { return mUltimateDamageTick; };
    void SetUltimateResource(int resource) { mUltimateResource = resource; }
    void EnableUltimate(bool enable);
    void EnableChargeUltimate(bool enable);
    void UltimateInterpolateLookAt(const float3& target); 

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
    UltimateState* GetUltimateState() { return mUltimateState; }


private:
    void CheckInput();
    void CheckHitEffect();
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
    UltimateState* mUltimateState = nullptr;
    UltimateChargeState* mUltimateChargeState = nullptr;

    // MOUSE
    float3 mPlayerDirection;
    float3 mAimPosition;

    // ANIMATION
    AnimationComponent* mAnimationComponent = nullptr;
    AnimationStateMachine* mStateMachine = nullptr;

    // STATS
    PlayerStats* mPlayerStats = nullptr;

    // Dash
    float mDashCoolDown = 2.0f;
    float mDashDuration = 0.5f;
    float mDashRange = 5.0f;

    // Speed
    float mPlayerSpeed;

    // Shield
    float mShield = 100.0f;
    float mMaxShield = 100.0f;
    GameObject* mHealParticles = nullptr;
    GameObject* mShieldSpriteSheet = nullptr;

    // WEAPONS
    Weapon* mWeapon = nullptr;
    Weapon* mSpecialWeapon = nullptr;
    int mCurrentEnergy = 100;
    EnergyType mEnergyType = EnergyType::NONE;
    int mUltimateResource = 100;
    float mDamageModifier = 1.0f;

    // RANGED
    RangeWeapon* mPistol = nullptr;
    RangeWeapon* mMachinegun = nullptr;
    RangeWeapon* mShootgun = nullptr;
    GameObject* mShootOrigin = nullptr;
    GameObject* mRedBaterryParticles = nullptr;
    GameObject* mBlueBaterryParticles = nullptr;

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
    float mGrenadeCursorSpeed = 6.0f;
    float3 mGrenadePosition;
    Grenade* mGrenade = nullptr;
    GameObject* mGrenadeGO = nullptr;
    GameObject* mGrenadeExplotionPreviewAreaGO = nullptr;

    //Ultimate
    GameObject* mUltimateGO = nullptr;
    GameObject* mUltimateChargeGO = nullptr;
    float mUltimateCooldown = 1.0f;
    float mUltimateChargeDuration = 1.0f;
    float mUltimateDuration = 3.0f;
    float mUltimatePlayerSlow = 1.0f;
    float mUltimateDamageTick = 1.0f;
    float mUltimateDamageInterval = 1.0f;
    float mUltimateAimSpeed = 1.0f;
    TimerScript UltimateRotationTimer;
    
    // Collider
    BoxColliderComponent* mCollider = nullptr;
    float3 mCollisionDirection = float3::zero;

    // Camera
    GameObject* mCamera = nullptr;

    // Debug
    bool mGodMode = false;

    //Hit Effect
    TimerScript  mHitEffectTimer;
    float mHitEffectTime = 0.15f;
    bool mHit = false;
    std::vector<Component*> mMeshComponents;
    std::vector<unsigned int> mMaterialIds;
    bool Delay(float delay);

    std::vector<float4> mPlayerOgColor;
 
    // DEBUFF
    bool mIsParalyzed = false;
    const float mParalyzedDuration = 5.0f;
    TimerScript mParalyzedTimerScript;
    float mParalysisSpeedReductionFactor = 1.0f;

    //Dash VFX
    GameObject* mDashVFX = nullptr;
};