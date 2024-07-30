#include "AttackState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Weapon.h"
#include "ModuleDetourNavigation.h"
#include "MeleeWeapon.h"


AttackState::AttackState(PlayerController* player, float cooldown) : State(player, cooldown)
{
}

AttackState::~AttackState()
{
}

StateType AttackState:: HandleInput()
{
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;
    if (mWeapon->GetCurrentAmmo() == 0) return StateType::RELOAD;

    mAttackTimer += App->GetDt();
    if (mAttackTimer < mWeapon->GetAttackDuration())
    {
        return StateType::ATTACK;
    }
       
    return StateType::AIM;
}

void AttackState::Update()
{
    mWeapon->Attack(mAttackTimer);
}

void AttackState::Enter()
{
    mAttackTimer = 0.0f;
    mWeapon = mPlayerController->GetWeapon();
    if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE)
    {
        mPlayerController->SetSpineAnimation("tAttackRanged", 0.01f);
    }
    else
    {
        mPlayerController->SetSpineAnimation("tAttackMelee", 0.2f);
        mPlayerController->SetAnimationSpeed(5.0f);
    }

    mWeapon->Enter();
    mAttackWhenPossible = false;
}

void AttackState::Exit()
{
    if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE)
    {
        mPlayerController->SetSpineAnimation("tIdleRanged", 0.01f);
    }
    else
    {
        mPlayerController->SetSpineAnimation("tIdleMelee", 0.01f);
        mPlayerController->SetAnimationSpeed(1.0f);
    }

    mWeapon->Exit();
    mWeapon = nullptr;
}

StateType AttackState::GetType()
{
    return StateType::ATTACK;
}

bool AttackState::IsReady()
{
    mStateTimer += App->GetDt();
    if (mStateTimer >= mStateCooldown) 
    {
        if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE && mPlayerController->GetWeapon()->GetCurrentAmmo() != 0)
        {
            return true;
        }
        if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::MELEE)
        {
            return true;
        }
    }
    else if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE && mPlayerController->GetWeapon()->GetCurrentAmmo() != 0 && App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
    {
        //This is neccesary to get an stable shoot time when clicking fast
        mAttackWhenPossible = true;
    }
    return false;
}
