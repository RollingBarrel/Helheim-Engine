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
	//Keep returning ultimate until timer reaches end 
	if (!mTimer.Delay(3.0f))
		return StateType::ULTIMATE;
	else 
		return StateType::AIM;
}

void UltimateState::Update()
{
}

void UltimateState::Enter()
{
	//mPlayerController->SetUltimateResource(0);
	mPlayerController->EnableUltimate(true);
	mPlayerController->SetMovementSpeed(0.0f);
}

void UltimateState::Exit()
{
	mPlayerController->EnableUltimate(false);
	//mPlayerController->SetMovementSpeed(0.0f);
}

StateType UltimateState::GetType()
{
	return StateType::ULTIMATE;
}

