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

enum class PlayerState 
{
    IDLE,
    DASH,
    MOVE,
    ATTACK,
    MOVE_ATTACK,
    DEATH
};

enum class BattleSituation {
    IDLE_HIGHT_HP,
    IDLE_LOW_HP,
    BATTLE_HIGHT_HP,
    BATTLE_LOW_HP,
    DEATH
};

enum class Weapon {
    RANGE,
    MELEE
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

        void TakeDamage(float damage);
        bool IsDead();

        BattleSituation GetBattleSituation() {return mCurrentSituation;};

    private:
        void Idle();
        void Moving();
        bool IsMoving();
        void Dash();
        void Attack();

        void MeleeAttack();
        void MeleeBaseCombo();
        void MeleeHit(float AttackRange, float AttackDamage);
        void RangedAttack();
        void Move(float3 position);
        void HandleRotation();
        void Shoot(float damage);
        void Reload();
        
        void Death();
        void UpdateShield();
        void UpdateBattleSituation();
        void CheckDebugOptions();

        void Victory();
        void GameoOver();
        bool Delay(float delay);
        void Loading();

        Weapon mWeapon = Weapon::RANGE;
        PlayerState mCurrentState = PlayerState::IDLE;
        PlayerState mPreviousState = PlayerState::IDLE;
        BattleSituation mCurrentSituation = BattleSituation::IDLE_HIGHT_HP;
        float mBattleStateTransitionTime = 0.0f;

        NavMeshController* mNavMeshControl = nullptr;
        GameObject* mAnimationComponentHolder = nullptr;
        AnimationComponent* mAnimationComponent = nullptr;
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

        //Dash
        bool mIsDashing = false;
        bool mIsDashCoolDownActive = false;
        float mDashCoolDownTimer = 0.0f;
        float mDashCoolDown = 0.7f;
        float mDashTimer = 0.0f;
        float mDashDuration = 0.5f;
        float mDashRange = 8.0f;
        float3 mDashDirection = float3::zero;

        //Range
        int mAmmoCapacity = 500000;
        int mBullets = 0;
        float mFireRate = 1.0f;
        float mRangeBaseDamage = 1.0f;
        float mRangeChargeAttackMultiplier = 5.0f;
        float mMinRangeChargeTime = 5.0f;
        float mMaxRangeChargeTime = 10.0f;
        GameObject* bullet = nullptr;

        float mChargedTime = 0.0f;
        bool mIsChargedAttack = false;

        float mRangeTimer = 0.0f;

        //Melee
       float mMeleeBaseDamage = 2.0f;
        float mMeleeBaseRange = 1.0f;
        /*
        float mMeleeBaseTimer = 0.0f;
        float mMeleeAttackDuration = 15;
        float mMeleeBasicComboCounter = 0;*/ 
        int mMeleeBaseComboStep = 0;
        float mMeleeBaseComboTimer = 0.0f;
        const float mMeleeBaseMaxComboInterval = 5.0f; // Maximum time interval between combo hits
        bool mIsMeleeBaseComboActive = false;


        // Combo timing array
        float3 mMeleeComboTiming = float3(0.5f, 0.5f, 0.75f );

        bool mIsAttacking = false;

  

        //HUD
        GameObject* mShieldGO = nullptr;
        SliderComponent* mShieldSlider = nullptr;


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
};