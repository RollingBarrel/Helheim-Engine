#include "State.h"

#include "PlayerController.h"
#include "Application.h"

State::State(PlayerController* player, float cooldown)
{
	mPlayerController = player;
	mStateCooldown = cooldown;
}

State::~State()
{
}

StateType State::HandleInput()
{
	return NONE;
}

void State::Update()
{
}

void State::Enter()
{
}

void State::Exit()
{
}

StateType State::GetType()
{
	return StateType::NONE;
}

bool State::IsReady()
{
	if (mStateTimer.DelayWithoutReset(mStateCooldown)) return true;
	return false;
}
