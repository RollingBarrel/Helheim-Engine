#include "AttackState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Weapon.h"
#include "ModuleDetourNavigation.h"
#include "MeleeWeapon.h"


AttackState::AttackState(PlayerController* player) : State(player)
{
}

AttackState::~AttackState()
{
}

StateType AttackState::HandleInput()
{
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;

    mSpecialAttackTimer += App->GetDt();
    if (mSpecialAttackTimer < mWeapon->GetAttackTime())
    {
        // MOVE TO WEAPON
        /*if (mWeapon->GetType() == Weapon::WeaponType::MELEE and
            App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
        {
            reinterpret_cast<MeleeWeapon*>(mWeapon)->IncreaseComboStep();
        }*/ 
        return StateType::ATTACK;
    }
       
    return StateType::AIM;
}

void AttackState::Update()
{
    mWeapon->Attack(mSpecialAttackTimer);
}

void AttackState::Enter()
{
    mSpecialAttackTimer = 0.0f;
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