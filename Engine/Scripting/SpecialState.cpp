#include "SpecialState.h"

SpecialState::SpecialState(PlayerController* player) : State(player)
{
}

SpecialState::~SpecialState()
{
}

StateType SpecialState::HandleInput()
{
	return StateType::AIM;
}

void SpecialState::Update()
{
}

void SpecialState::Enter()
{
}

void SpecialState::Exit()
{
}

StateType SpecialState::GetType()
{
	return StateType::SPECIAL;
}

void SpecialState::PlayAudio()
{
}