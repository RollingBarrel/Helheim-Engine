#include "AttackState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Weapon.h"

AttackState::AttackState(PlayerController* player) : State(player)
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
    mWeapon->BasicAttack();
    DoAudio();
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
