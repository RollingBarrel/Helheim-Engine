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

enum class BattleSituation {
    IDLE_HIGHT_HP,
    IDLE_LOW_HP,
    BATTLE_HIGHT_HP,
    BATTLE_LOW_HP,
    DEATH
};

GENERATE_BODY(PlayerController);
class PlayerController :public Script
{
    FRIEND(PlayerController)
public:
    PlayerController(GameObject* owner);
    ~PlayerController();
    void Start() override;
    void Update() override;

    float3 GetPlayerDirection() { return mPlayerDirection; }
    float3 GetPlayerAimPosition() { return mAimPosition; }
    float3 GetPlayerPosition();
   
    void SetAnimation(std::string trigger, float transitionTime);
    void PlayOneShot(std::string name);

    void MoveToPosition(float3 position);
    void MoveInDirection(float3 direction);

    void SwitchWeapon();

    float GetDashCoolDown() const { return mDashCoolDown; }
    float GetDashDuration() const { return mDashDuration; }
    float GetDashRange() const { return mDashRange; }
    float GetGrenadeCooldown() const { return mGrenadeCoolDown; }
    float GetGrenadeRange() const { return mGrenadeRange;  }
    float GetAttackCooldown() const { return mAttackCoolDown; }
    float GetSpecialAttackCooldown() const { return mSpecialAttackCoolDown; }
    float GetSwitchCooldown() const { return mSwitchCoolDown; }
    float GetReloadDuration() const { return mReloadDuration; }
    Weapon* GetWeapon() const { return mWeapon; }
    State* GetPlayerUpperState() const { return mUpperState; }
    
    bool CanReload() const;
    void Reload() const;

    void SetDashCoolDown(float value) { mDashCoolDown = value; }
    void SetDashDuration(float value) { mDashDuration = value; }
    void SetDashRange(float value) { mDashRange = value; }
    void SetGrenadeCooldown(float value) { mGrenadeCoolDown = value; }
    void SetGrenadeRange(float value) { mGrenadeRange = value; }

    // --------------- OLD ----------------------

    void RechargeShield(float shield);
    void TakeDamage(float damage);

    BattleSituation GetBattleSituation() { return mCurrentSituation; };

private:
    void CheckInput();
    void StateMachine();

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
    MeleeWeapon* mMeleeWeapon = nullptr;
    RangeWeapon* mRangeWeapon = nullptr;
    // Attack
    float mAttackCoolDown = 0.1f;
    float mSpecialAttackCoolDown = 5.0f;
    float mSwitchCoolDown = 0.2f;
    float mReloadDuration = 0.5;
    // Grenade
    float mGrenadeCoolDown = 5.0f;
    float mGrenadeRange = 5.0f;
    Grenade* mGrenade = nullptr;
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
    void CheckDebugOptions();
    void UpdateShield();
    void UpdateBattleSituation();
    void HandleRotation();


    bool mGodMode = false;

    GameObject* mCamera = nullptr;

    BoxColliderComponent* mCollider = nullptr;

    BattleSituation mCurrentSituation = BattleSituation::IDLE_HIGHT_HP;
    float mBattleStateTransitionTime = 0.0f;
};