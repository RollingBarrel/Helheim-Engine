#include "UltimateState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"

UltimateState::UltimateState(PlayerController* player, float cooldown) : State(player, cooldown)
{
}

UltimateState::~UltimateState()
{
}

StateType UltimateState::HandleInput()
{
	return StateType::ULTIMATE;
}

void UltimateState::Update()
{
}

void UltimateState::Enter()
{
}

void UltimateState::Exit()
{
}

StateType UltimateState::GetType()
{
	return StateType::ULTIMATE;
}

