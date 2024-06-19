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
    

    if (mAttackTimer < mWeapon->GetAttackTime())
    {
        return StateType::ATTACK;
    }
    else
    {
        mAttackTimer = 0;
        if (mWeapon->GetType() == Weapon::WeaponType::RANGE && mWeapon->GetCurrentAmmo() == 0)
        {
            return StateType::RELOAD;
        }
        if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN || App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT)
        {
            return StateType::ATTACK;
        }
        return StateType::AIM;
    }
}

void AttackState::Update()
{
    if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE)
    {
        mWeapon->Attack();
    }else 
    { 
        bool firstHit = false;
        if (!mIsComboActive)
        {
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
            mWeapon->Attack();
            mComboStep = 3;
            mIsNextComboStep = true;
            mBreakCombo = 0.0f;
        } 
        else if (mComboCurrentTime > mComboMilestone2 && mComboCurrentTime < mComboDuration && mComboStep == 3 && mIsNextComboStep)
        {
            if (!mDashActive)
            {
                mDashActive = true;
                mDashTimer = 0.0f;
                mDashDirection = mPlayerController->GetPlayerDirection(); 
            }

            mDashTimer += App->GetDt();

            float dashSpeed = mMoveRange / mMoveDuration;
            float3 currentPos = mPlayerController->GetPlayerPosition();
            float3 futurePos = currentPos + mDashDirection * dashSpeed * App->GetDt(); 

            mPlayerController->MoveToPosition(futurePos);

            if (mDashTimer >= mMoveDuration)
            {
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
}



void AttackState::Enter()
{
    mAttackTimer = 0;
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

void AttackState::PlayAudio()
{

}
