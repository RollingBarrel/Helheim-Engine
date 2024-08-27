#pragma once
#include "State.h"
#include "float3.h"

class Weapon;

class AttackState : public State
{
public:
	explicit AttackState(PlayerController* player, float cooldown);
	~AttackState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

    bool IsReady() override;
    bool CanAttackWhenPossible() { return mAttackWhenPossible; }

private:
	Weapon* mWeapon = nullptr;
	float mAttackTimer = 0.0f;

    float3 mDashDirection;
    float mEnergyCost = 10.0f;
    float mCooldownModifier = 1.f;

    float mComboTimer = 0.0f;
    float mComboCurrentTime = 0.0f;
    int   mComboStep = 0;
    bool  mIsComboActive = false;
    float mDashTimer = 0.0f;
    bool  mDashActive = false;

    bool  mIsNextComboStep = false;
    float mBreakCombo = 0.0f;

    float mComboMilestone1 = 1.0f;
    float mComboMilestone2 = 2.0f;
    float mComboDuration = 3.0f;

    bool mAttackWhenPossible = false;
    TimerScript mPressedAttackTimer;
    float mAttackPressedCoolDown = 0.4f;

    // Player Dash
    float  mMoveRange = 10.f;
    float  mMoveDuration = 2.0f;

};