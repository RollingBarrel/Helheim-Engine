#include "AttackState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Weapon.h"
#include "ModuleDetourNavigation.h"


AttackState::AttackState(PlayerController* player) : State(player), mPlayerController(player)
{
}

AttackState::~AttackState()
{
}

StateType AttackState::HandleInput()
{
    mAttackTimer += App->GetDt();

    if (mAttackTimer < mWeapon->GetAttackTime())
    {
        return StateType::ATTACK;
    }
    else
    {
        return StateType::AIM;
    }
}

void AttackState::Update()
{
    bool firstHit = false;
    if (!mIsComboActive)
    {
        // Primer ataque
        LOG("First MELEE ATTACK!!");
        mWeapon->Attack();
        mIsComboActive = true;
        mComboCurrentTime = 0.0f;
        mComboStep = 2;
        firstHit = true;
    }

    if (!mIsNextComboStep)
    {
        mIsNextComboStep = !firstHit;
        mBreakCombo = 0.0f;
    }

    mComboCurrentTime += App->GetDt();

    if (mComboCurrentTime > mComboMilestone1 && mComboCurrentTime < mComboMilestone2 && mIsNextComboStep && mComboStep == 2)
    {
        // Segundo ataque
        LOG("Second MELEE ATTACK!!");
        mWeapon->Attack();
        mComboStep = 3;
        mIsNextComboStep = true;
        mBreakCombo = 0.0f;
    }
    else if (mComboCurrentTime > mComboMilestone2 && mComboCurrentTime < mComboDuration && mComboStep == 3 && mIsNextComboStep)
    {
        // Ataque con deslizamiento
        if (!mDashActive)
        {
            LOG("Starting Dash for MELEE ATTACK!!");
            mDashActive = true;
            mDashTimer = 0.0f;
            mDashDirection = mPlayerController->GetPlayerDirection(); // Fijar la dirección al inicio del dash
        }

        mDashTimer += App->GetDt();

        float dashSpeed = mMoveRange / mMoveDuration;
        float3 currentPos = mPlayerController->GetPlayerPosition();
        float3 futurePos = currentPos + mDashDirection * dashSpeed * App->GetDt(); // Usar dirección fijada

        // Movimiento del jugador
        mPlayerController->MoveToPosition(futurePos);

        if (mDashTimer >= mMoveDuration)
        {
            // Ataque final después del deslizamiento
            LOG("Final MELEE ATTACK after DASH!!");
            mWeapon->Attack();
            mIsComboActive = false;
            mComboCurrentTime = 0.0f;
            mComboStep = 0;
            mIsNextComboStep = false;
            mBreakCombo = 0.0f;
            mDashActive = false;
        }
    }
    else if (mComboCurrentTime > mComboDuration && mComboStep == 3 && mIsNextComboStep)
    {
        // Fin del combo
        mIsComboActive = false;
        mComboCurrentTime = 0.0f;
        mComboStep = 0;
        mIsNextComboStep = false;
        mBreakCombo = 0.0f;
        mDashActive = false;
    }
    else if (!mIsNextComboStep)
    {
        mBreakCombo += App->GetDt();
    }

    float breakTime = 10.0f;
    switch (mComboStep)
    {
    case 1:
        breakTime = 10.0f;
        break;
    case 2:
        breakTime = 1.1f;
        break;
    case 3:
        breakTime = 0.9f;
        break;
    default:
        break;
    }

    if (mBreakCombo > breakTime)
    {
        mIsComboActive = false;
        mComboCurrentTime = 0.0f;
        mComboStep = 1;
        mIsNextComboStep = false;
        mBreakCombo = 0.0f;
        mDashActive = false;
    }

    if (mComboCurrentTime > mComboDuration + 2.0f)
    {
        mComboCurrentTime = 0.0f;
    }
}



void AttackState::Enter()
{
    mWeapon = mPlayerController->GetWeapon();
    mWeapon->Enter();
}

void AttackState::Exit()
{
    mWeapon->Exit();
    mWeapon = nullptr;
}

StateType AttackState::GetType()
{
    return StateType::ATTACK;
}

void AttackState::DoAudio()
{
}
