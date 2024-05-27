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
class GameManager;
class AnimationStateMachine;
class BoxColliderComponent;
class HudController;
class Grenade;
struct CollisionData;

class RangeWeapon;

enum class PlayerState 
{
    IDLE,
    DASH,
    MOVE,
    ATTACK,
    MOVE_ATTACK,
    DEATH,
};

enum class BattleSituation {
    IDLE_HIGHT_HP,
    IDLE_LOW_HP,
    BATTLE_HIGHT_HP,
    BATTLE_LOW_HP,
    DEATH
};

enum class WeaponType {
    RANGE,
    MELEE
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

        void RechargeShield(float shield);
        void TakeDamage(float damage);
        bool IsDead();

        BattleSituation GetBattleSituation() {return mCurrentSituation;};

    private:
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
        void Move(float3 position);
        void HandleRotation();
        void Shoot(float damage);
        void Reload();
        void ClosestMouseDirection(const float2& mouseState); 
        void SetMovingDirection(const float3& moveDirection);
      
        void Death();
        void UpdateShield();
        void UpdateBattleSituation();
        void CheckDebugOptions();

        void UpdateGrenadeCooldown();
        void GrenadeAttack();
        void AimGrenade();
        void GrenadeTarget();
        void ThrowGrenade(float3 target);

        void Victory();
        void GameOver();
        bool Delay(float delay);
        void Loading();
        
        void OnCollisionEnter(CollisionData* collisionData);

        WeaponType mWeapon = WeaponType::RANGE;
        PlayerState mCurrentState = PlayerState::IDLE;
        PlayerState mPreviousState = PlayerState::IDLE;
        BattleSituation mCurrentSituation = BattleSituation::IDLE_HIGHT_HP;
        MouseDirection mLookingAt = MouseDirection::DEFAULT;
        MoveDirection mMovingTo = MoveDirection::NOT_MOVING;
        float mBattleStateTransitionTime = 0.0f;

        NavMeshController* mNavMeshControl = nullptr;
        AnimationComponent* mAnimationComponent = nullptr;
        AnimationStateMachine* mStateMachine = nullptr;

        GameObject* mBulletPoolHolder = nullptr;
        ObjectPool* mBulletPool = nullptr;

        GameManager* mGameManager = nullptr;
        GameObject* mGameManagerGO = nullptr;

        //Stats
        float mPlayerSpeed = 2.0f;
        float mShield = 0.0f;
        float mMaxShield = 100.0f;
        float mSanity = 0.0f;
        float mMaxSanity = 100.0f;
        bool mPlayerIsDead = false;
        float3 mMoveDirection = float3::zero;

        //Dash
        bool mIsDashing = false;
        bool mIsDashCoolDownActive = false;
        float mDashCoolDownTimer = 0.0f;
        float mDashCoolDown = 0.7f;
        float mDashTimer = 0.0f;
        float mDashDuration = 0.5f;
        float mDashRange = 8.0f;

        //Range
        int mAmmoCapacity = 500000;
        int mBullets = 0;
        //GameObject* bullet = nullptr;
        GameObject* mRangeWeaponGameObject = nullptr;
        RangeWeapon* mRangeWeapon = nullptr;

        float mRangeBaseDamage = 1.0f;

        //Melee
        bool mLeftMouseButtonPressed = false;

        //Melee Base Attack
        float mMeleeBaseDamage = 2.0f;
        float mMeleeBaseRange = 1.0f;
               
        //Combo
        int mMeleeBaseComboStep = 1;
        float mMeleeBaseComboTimer = 0.0f;
        const float mMeleeBaseMaxComboInterval = 5.0f; 
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
       
        float mGrenadThrowDistance = 5.0f;  // mGrenadeAimAreaGO radius
        float mGrenadeCoolDown = 5.0f;
        float mGrenadeCoolDownTimer = mGrenadeCoolDown;

        Grenade* mGrenade = nullptr;

        GameObject* mGrenadeAimAreaGO = nullptr;
        GameObject* mGrenadeExplotionPreviewAreaGO = nullptr;

        //HUD
        GameObject* mHudControllerGO = nullptr;
        HudController* mHudController = nullptr;

        //DEBUG
        bool mGodMode = false;

        GameObject* mWinArea = nullptr;

        // Audios section
        // Footstep
        GameObject* mFootStepAudioHolder = nullptr;
        AudioSourceComponent* mFootStepAudio = nullptr;
        bool mIsMoving = false;
        bool mReadyToStep = false;
        float mStepTimePassed = 0.0f;
        float mStepCoolDown = 0.5f;

        // Gunfire
        GameObject* mGunfireAudioHolder = nullptr;
        AudioSourceComponent* mGunfireAudio = nullptr;

        //SCREENS
        bool mVictory = false;
        bool mGameOver = false;
        bool mLoadingActive = false;
        float mTimeScreen = 3.0f;
        float mTimePassed = 0.0f;

        //CAMERA
        GameObject* mCamera = nullptr;
        //Collider
        BoxColliderComponent* mCollider;
};