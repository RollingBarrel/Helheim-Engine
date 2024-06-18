#include "AttackState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"

AttackState::AttackState(PlayerController* player) : State(player)
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
        return StateType::AIM;
    }
}

void AttackState::Update()
{
    mAttackTimer += App->GetDt();
    mWeapon->BasicAttack(mAttackTimer);
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
