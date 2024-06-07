#include "State.h"

#include "PlayerController.h"

State::State(PlayerController* player)
{
	mPlayerController = player;
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
