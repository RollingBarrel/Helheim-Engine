#pragma once
#include "Script.h"
#include "Macros.h"
#include "float3.h"

class NavMeshController;
class AnimationComponent;
class AudioSourceComponent;
class SliderComponent;
class AudioSourceComponent;
class ObjectPool;
class AnimationStateMachine;
class BoxColliderComponent;
class HudController;
class Grenade;
struct CollisionData;


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

enum class BattleSituation {
    IDLE_HIGHT_HP,
    IDLE_LOW_HP,
    BATTLE_HIGHT_HP,
    BATTLE_LOW_HP,
    DEATH
};

enum class MouseDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT,
    UP_RIGHT,
    UP_LEFT,
    DOWN_RIGHT,
    DOWN_LEFT,
    DEFAULT
};

enum class MoveDirection {
    UP,
    DOWN,
    RIGHT,
    LEFT,
    UP_RIGHT,
    UP_LEFT,
    DOWN_RIGHT,
    DOWN_LEFT,
    NOT_MOVING
};

GENERATE_BODY(PlayerController);
class PlayerController :public Script
{
    FRIEND(PlayerController)
public:
    PlayerController(GameObject* owner);
    ~PlayerController() {}
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

    // STATS
    // Dash
    float mDashCoolDown = 0.7f;
    float mDashDuration = 0.5f;
    float mDashRange = 5.0f;
    // Speed
    float mPlayerSpeed = 2.0f;

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

    // -------- OLD ---------------

    void Idle();
    void Moving();
    void Dash();
    void Attack();
    void InitAnimations();

    void MeleeAttack();
    void MeleeBaseCombo();
    void MeleeSpecialCombo();

    void MeleeHit(float AttackRange, float AttackDamage);
    void RangedAttack();
    //void Move(float3 position);
    void HandleRotation();
    void Shoot(float damage);
    void ClosestMouseDirection(const float2& mouseState);
    void SetMovingDirection(const float3& moveDirection);

    void UpdateShield();
    void UpdateBattleSituation();
    void CheckDebugOptions();

    void UpdateGrenadeCooldown();
    void GrenadeAttack();
    void AimGrenade();
    void GrenadeTarget();
    void ThrowGrenade(float3 target);

    // Game State
    void Victory();
    void GameOver();

    // Utils
    bool Delay(float delay);

    void OnCollisionEnter(CollisionData* collisionData);

    BattleSituation mCurrentSituation = BattleSituation::IDLE_HIGHT_HP;
    MouseDirection mLookingAt = MouseDirection::DEFAULT;
    MoveDirection mMovingTo = MoveDirection::NOT_MOVING;
    float mBattleStateTransitionTime = 0.0f;

    NavMeshController* mNavMeshControl = nullptr;
    AnimationStateMachine* mStateMachine = nullptr;

    GameObject* mBulletPoolHolder = nullptr;
    ObjectPool* mBulletPool = nullptr;

    //Stats
    float mShield = 0.0f;
    float mMaxShield = 100.0f;
    float mSanity = 0.0f;
    float mMaxSanity = 100.0f;
    float3 mMoveDirection = float3::zero;

    //Dash
    bool mIsDashing = false;
    bool mIsDashCoolDownActive = false;

    //Range
    int mAmmoCapacity = 500000;
    int mBullets = 0;
    //GameObject* bullet = nullptr;
    float mShootingTimer = 0.0f;
    bool mHasShoot = false;
    float mRangeBaseDamage = 1.0f;

    //Melee
    bool mLeftMouseButtonPressed = false;

    //Melee Base Attack
    float mMeleeBaseDamage = 2.0f;
    float mMeleeBaseRange = 1.0f;

    //Combo
    int mMeleeBaseComboStep = 1;
    float mMeleeBaseComboTimer = 0.0f;
    const float mMeleeBaseMaxComboInterval = 1.0f;
    float mMeleeComboDuration = 2.6f;
    float mMeleeComboMilestone1 = 1.1f;
    float mMeleeComboMilestone2 = 2.0f;
    float mMeleComboCurrentTime = 0.0f;
    float mBreakMeleeCombo = 0.0f;
    bool mNextComboStep = false;
    bool mIsMeleeBaseComboActive = false;


    //Final Attack
    const float mMeleeBaseFinalAttackDuration = 0.5f;
    float mMeleeBaseFinalAttackTimer = 0.0f;
    float mMeleeBaseMoveDuration = 0.5f;
    float mMeleeBaseMoveRange = 8.0f;

    //Melee Special Attack
    float mMeleeSpecialTimer = 0.0f;
    const float mMeleeSpecialAttackDuration = 2.0f;
    float mMeleeSpecialDamage = 4.0f;
    float mMeleeSpecialRange = 2.0f;

    //Cooldown
    bool mIsMeleeSpecialCoolDownActive = false;
    float mMeleeSpecialCoolDownTimer = 0.0f;
    float mMeleeSpecialCoolDown = 4.0f;

    // Grenade
    bool mAimingGrenade = false;
    bool mThrowAwayGrenade = false;

    
    float mGrenadeCoolDownTimer = mGrenadeCoolDown;


    //DEBUG
    bool mGodMode = false;

    GameObject* mWinArea = nullptr;

    // Audios section
    bool mIsMoving = false;
    bool mReadyToStep = false;
    float mStepTimePassed = 0.0f;
    float mStepCoolDown = 0.5f;


    //SCREENS
    bool mVictory = false;
    bool mGameOver = false;
    bool mLoadingActive = false;
    float mTimeScreen = 3.0f;

    //CAMERA
    GameObject* mCamera = nullptr;
    //Collider
    BoxColliderComponent* mCollider = nullptr;

    // Timer
    float mTimePassed = 0.0f;

};