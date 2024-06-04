#include "AimState.h"

AimState::AimState(PlayerController* player) : State(player)
{
}

AimState::~AimState()
{
}

StateType AimState::HandleInput()
{
	return State::HandleInput();
}

void AimState::Update()
{
}

void AimState::Enter()
{
}

void AimState::Exit()
{
}
