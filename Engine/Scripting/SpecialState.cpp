#include "SpecialState.h"

#include "Application.h"
#include "PlayerController.h"

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
	mPlayerController->SetSpineAnimation("tSpecial", 0.1f);
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