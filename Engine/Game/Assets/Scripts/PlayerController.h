#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"

class AnimationComponent;
class AnimationStateMachine;
class AudioSourceComponent;
struct CollisionData;
class BoxColliderComponent;

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

enum class BatteryType 
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
    float3 GetPlayerAimPosition() { return mAimPosition; }
    float3 GetPlayerPosition();

    Weapon* GetMeleeWeapon() const { return mBat; }
    Weapon* GetRangeWeapon() const { return mPistol; }
   
    void SetAnimation(std::string trigger, float transitionTime);
    void SetSpineAnimation(std::string trigger, float transitionTime);
    void PlayOneShot(std::string name);

    void MoveToPosition(float3 position);
    void MoveInDirection(float3 direction);

    void SwitchWeapon();
    void SwitchWeapon(Weapon* newWeapon);

    float GetDashCoolDown() const { return mDashCoolDown; }
    float GetDashDuration() const { return mDashDuration; }
    float GetDashRange() const { return mDashRange; }
    float GetGrenadeCooldown() const { return mGrenadeCoolDown; }
    float GetGrenadeRange() const { return mGrenadeRange;  }
    float GetAttackCooldown() const { return mAttackCoolDown; }
    float GetSlowAttackCooldown() const { return mSlowAttackCoolDown; }
    float GetSpecialAttackCooldown() const { return mSpecialAttackCoolDown; }
    float GetSwitchCooldown() const { return mSwitchCoolDown; }
    float GetReloadDuration() const { return mReloadDuration; }
    Weapon* GetWeapon() const { return mWeapon; }
    Weapon* GetSpecialWeapon() const { return mSpecialWeapon; }
    float GetCurrentBattery() const { return mCurrentBattery; }
    BatteryType GetBatteryType() const { return mBatteryType; }
    const State* GetPlayerUpperState() const { return mUpperState; }

    void SetSpecialWeapon(Weapon* weapon) { mSpecialWeapon = weapon; }
    void SetDashCoolDown(float value) { mDashCoolDown = value; }
    void SetDashDuration(float value) { mDashDuration = value; }
    void SetDashRange(float value) { mDashRange = value; }
    void SetGrenadeCooldown(float value) { mGrenadeCoolDown = value; }
    void SetGrenadeRange(float value) { mGrenadeRange = value; }
    void SetGrenadeVisuals(bool value);
    void UpdateGrenadeVisuals();
    void ThrowGrenade();
    void SetPlayerPosition(float3 position);


    bool CanReload() const;
    void Reload() const;
    
    // --------------- OLD ----------------------

    void RechargeShield(float shield);
    void RechargeBattery(BatteryType batteryType);
    void UseEnergy(float energy);
    void TakeDamage(float damage);

    BattleSituation GetBattleSituation() { return mCurrentSituation; };

private:
    void CheckInput();
    void StateMachine();
    void HandleRotation();
    void CheckDebugOptions();

    // STATES
    State* mLowerState = nullptr;
    StateType mLowerStateType;

    DashState* mDashState;
    IdleState* mIdleState;
    MoveState* mMoveState;

    State* mUpperState = nullptr;
    StateType mUpperStateType;

    AimState* mAimState;
    AttackState* mAttackState;
    GrenadeState* mGrenadeState;
    SwitchState* mSwitchState;
    SpecialState* mSpecialState;
    ReloadState* mReloadState;

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
    float mCurrentBattery = 0.0f;
    BatteryType mBatteryType = BatteryType::NONE;

    // RANGED
    Weapon* mPistol = nullptr;
    Weapon* mMachinegun = nullptr;
    Weapon* mShootgun = nullptr;

    // MELEE
    Weapon* mBat = nullptr;
    Weapon* mKatana = nullptr;
    Weapon* mHammer = nullptr;
    GameObject* mMeleeCollider = nullptr;

    // Attack
    float mAttackCoolDown = 0.1f;
    float mSlowAttackCoolDown = 0.5f;
    float mSpecialAttackCoolDown = 5.0f;
    float mSwitchCoolDown = 0.2f;
    float mReloadDuration = 0.5;
    // Grenade
    float mGrenadeCoolDown = 5.0f;
    float mGrenadeRange = 5.0f;
    float3 mGrenadePosition;
    Grenade* mGrenade = nullptr;
    GameObject* mGrenadeGO = nullptr;
    GameObject* mGrenadeAimAreaGO = nullptr;
    GameObject* mGrenadeExplotionPreviewAreaGO = nullptr;

    // -------- PROVISIONAL --------

    // AUIDO
    // Footstep
    GameObject* mFootStepAudioHolder = nullptr;
    AudioSourceComponent* mFootStepAudio = nullptr;
    // Gunfire
    GameObject* mGunfireAudioHolder = nullptr;
    AudioSourceComponent* mGunfireAudio = nullptr;

    void OnCollisionEnter(CollisionData* collisionData);
    void UpdateBattleSituation();


    bool mGodMode = false;

    GameObject* mCamera = nullptr;

    BoxColliderComponent* mCollider = nullptr;

    BattleSituation mCurrentSituation = BattleSituation::IDLE_HIGHT_HP;
    float mBattleStateTransitionTime = 0.0f;
};