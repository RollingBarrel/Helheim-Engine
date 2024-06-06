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
	return StateType::AIM;

	//return State::HandleInput();
}

void AttackState::Update()
{
    Weapon* weapon = mPlayerController->GetWeapon();
    weapon->BasicAttack();

    DoAudio();
}

void AttackState::Enter()
{
}

void AttackState::Exit()
{
}

void AttackState::DoAudio()
{
}
