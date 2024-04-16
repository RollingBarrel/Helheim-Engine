#pragma once
#include <Script.h>
#include "Macros.h"
#include "Math/float3.h"

class NavMeshController;
class AnimationComponent;
class Gun;

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

        void WinTest();
        void LoseTest();

        //Stats variables
        float mPlayerSpeed = 2.0f;
        float mPlayerRotationSpeed = 0.5f;
        GameObject* mWinArea = nullptr;
        GameObject* mLoseArea = nullptr;
        GameObject* mAnimationComponentHolder = nullptr;

        //Dash Variables
        float mDashSpeed = 5.0f;//35
        float mDashDistance = 3.0f;
        float mDashCoolDown = 3.0f;
        int mDashCharges = 5;//3

        int mHealth = 1;
        int mShield = 100;
        int mSanity = 100;


    private:

        enum class PlayerState {
            Idle,
            Dash,
            Forward,
            Backward,
            Left,
            Right,
            MeleeAttack,
            RangedAttack,
            Reload,
            ThrowGrenade,
            Death
        };

        void ChangeState(PlayerState newState);
        void StateMachine();
        void Controls();
        void Forward();
        void Backward();
        void Left();
        void Right();
        void Move(float3 position);

        //Attack functions
        void MeleeAttack();
        void RangedAttack();

        void Shoot(bool isChargedShot, float chargeTime);
        void ShootLogic(int damage);
        void Reload();

        void ReloadWeapon();
        void ThrowGrenade();

        void Mouse_Rotation();
        void Dash();
        bool ShieldDamage(int damage);
        void Sanity();
        void Death();
        void WinMessage();
        void LoseMessage();
        void CheckRoute();

        NavMeshController* mNavMeshControl = nullptr;
        AnimationComponent* mAnimationComponent = nullptr;
        Gun* mGun = nullptr;

        PlayerState mCurrentState;
        PlayerState mPreviousState;

        //Dash variables
        bool mIsDashActive = false;
        bool mStartCounter = false;
        float mDashTimePassed = 0.0f;
        float mDashMovement = 0;
        bool mDashTrigger = false;

        bool mPlayerIsDead = false;
        bool mIsMoving = false;
        //bool mIsRecharging = false;
        //float mReloadTimePassed = 0.0f;

        //Shooting variables
        int mDamage = 1;
        int mMaxBullets = 16;
        int mBullets = 16;
        float mChargedShotTime = 10.0f;
        float mBulletCostPerSecond = 1.0f;
        bool mIsChargedShot = false;

        //Reload variables
        //float mReloadTime = 10.0f;
};