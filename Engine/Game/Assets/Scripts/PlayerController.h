#pragma once
#include <Script.h>
#include "Macros.h"

class NavMeshController;
class AnimationComponent;
class SliderComponent;

enum class PlayerState {
    IDLE,
    DASH,
    MOVE,
    ATTACK,
    MOVE_ATTACK,
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

        void Hit(float damage);
        bool IsDead();

    private:
        void Idle();
        void Moving();
        void Dash();
        void Attack();

        void MeleeAttack();
        void RangedAttack();
        void Move(float3 position);

        void Shoot(float damage);
        void Reload();
        
        void RechargeDash();
        void Death();
        void CheckRoute();
        void UpdateHealth();
        void CheckDebugOptions();

        Weapon mWeapon = Weapon::MELEE;
        PlayerState mCurrentState = PlayerState::IDLE;
        PlayerState mPreviousState = PlayerState::IDLE;

        NavMeshController* mNavMeshControl = nullptr;
        GameObject* mAnimationComponentHolder = nullptr;
        AnimationComponent* mAnimationComponent = nullptr;

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

};