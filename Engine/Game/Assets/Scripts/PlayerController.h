#pragma once
#include <Script.h>
#include "Macros.h"

class NavMeshController;
class AnimationComponent;

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

        void SetPlayerDamage(int damage);
        bool GetPlayerIsDead();

    private:
        void Idle();
        void Moving();
        void Dash();
        void Attack();


        void MeleeAttack();
        void RangedAttack();
        void Move(float3 position);

        
        void ShootLogic(int damage);
        void Reload();
        
        
        bool ShieldDamage(int damage);
        void RechargeDash();
        void Death();
        void CheckRoute();

        Weapon mWeapon = Weapon::MELEE;
        PlayerState mCurrentState = PlayerState::IDLE;
        PlayerState mPreviousState = PlayerState::IDLE;

        NavMeshController* mNavMeshControl = nullptr;
        GameObject* mAnimationComponentHolder = nullptr;
        AnimationComponent* mAnimationComponent = nullptr;

        //Dash variables
        bool mIsDashCoolDownActive = false;
        float mDashTimePassed = 0.0f;
        float mDashMovement = 0;
        int mMaxDashCharges = 3;
        int mDashCharges = 0;//3
        float mDashChargeRegenerationTime = 3.0f;
        float mDashSpeed = 35.0f;//35
        float mDashDistance = 5.0f;
        float mDashCoolDown = 3.0f;

        bool mPlayerIsDead = false;
        bool mIsMoving = false;

        //Shooting variables
        int mDamage = 1;
        int mMaxBullets = 500000;
        int mBullets = 50000;
        float mChargedShotTime = 10.0f;
        float mBulletCostPerSecond = 1.0f;
        bool mIsChargedAttack = false;

        float mPlayerSpeed = 2.0f;
        int mHealth = 1;
        int mShield = 100;
        int mSanity = 100;

};