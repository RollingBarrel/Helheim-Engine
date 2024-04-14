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
        bool PlayerIsDeath();

        void WinTest();
        void LoseTest();

        //Stats variables
        float mPlayerSpeed = 2.0f;
        float mPlayerRotationSpeed = 0.5f;
        GameObject* mWinArea = nullptr;
        GameObject* mLoseArea = nullptr;
        GameObject* mAnimationComponentHolder = nullptr;
        float mDashSpeed = 5.0f;
        float mDashLenght = 5.0f;
        float mDashCoolDown = 1.0f;
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

        void Shoot(int damage);
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
        Gun* Gun = nullptr;

        PlayerState mCurrentState;
        PlayerState mPreviousState;
        bool mIsDashActive = false;
        bool mStartCounter = false;
        float mDashTimePassed = 0.0f;
        float mDashMovement = 0;
        bool mPlayerIsDeath = false;
        bool mIsMoving = false;
        bool mIsRecharging = false;
        float mReloadTimePassed = 0.0f;

};