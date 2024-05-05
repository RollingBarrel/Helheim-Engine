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
        void Dash();
        void Attack();

        void MeleeAttack();
        void RangedAttack();
        void Move(float3 position);
        void HandleRotation();
        void Shoot(float damage);
        void Reload();
        
        void RechargeDash();
        void Death();
        void UpdateHealth();
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

        //Stats
        float mPlayerSpeed = 2.0f;
        float mHealth = 0.0f;
        float mMaxHealth = 100.0f;
        float mShield = 0.0f;
        float mMaxShield = 100.0f;
        float mSanity = 0.0f;
        float mMaxSanity = 100.0f;
        bool mPlayerIsDead = false;

        //Dash
        bool mIsDashCoolDownActive = false;
        float mDashTimePassed = 0.0f;
        float mDashMovement = 0;
        int mMaxDashCharges = 3;
        int mDashCharges = 0;//3
        float mDashChargeRegenerationTime = 3.0f;
        float mDashSpeed = 35.0f;//35
        float mDashDistance = 5.0f;
        float mDashCoolDown = 3.0f;

        //Range
        int mAmmoCapacity = 500000;
        int mBullets = 0;
        float mFireRate = 1.0f;
        float mRangeBaseDamage = 1.0f;
        float mRangeChargeAttackMultiplier = 5.0f;
        float mMinRangeChargeTime = 5.0f;
        float mMaxRangeChargeTime = 10.0f;
        GameObject* bullet = nullptr;

        float startingTime = 0.0F;

        //Melee
        float mMeleeBaseDamage = 1.0f;
        float mMeleeChargeAttackMultiplier = 5.0f;
        float mMinMeleeChargeTime = 5.0f;
        float mMaxMeleeChargeTime = 10.0f;
        
        float mChargedTime = 0.0f;
        bool mIsChargedAttack = false;

        //HUD
        GameObject* mHealthGO = nullptr;
        GameObject* mDashGO_1 = nullptr;
        GameObject* mDashGO_2 = nullptr;
        GameObject* mDashGO_3 = nullptr;
        SliderComponent* mHealthSlider = nullptr;
        SliderComponent* mDashSlider_1 = nullptr;
        SliderComponent* mDashSlider_2 = nullptr;
        SliderComponent* mDashSlider_3 = nullptr;

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