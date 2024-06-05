#include "AttackState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"

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
}

void AttackState::Enter()
{
}

void AttackState::Exit()
{
}
