#include "UltimateChargeState.h"
#include "PlayerController.h"

UltimateChargeState::UltimateChargeState(PlayerController* player, float cooldown, float duration) : State(player, cooldown)
{
	mUltimateChargeDuration = duration;
}

UltimateChargeState::~UltimateChargeState()
{
}

StateType UltimateChargeState::HandleInput()
{
	if (!mTimer.Delay(mUltimateChargeDuration))
		return StateType::ULTIMATE_CHARGE;
	else
		return StateType::ULTIMATE;
}

void UltimateChargeState::Update()
{
}

void UltimateChargeState::Enter()
{
	mUltimateChargeDuration = mPlayerController->GetUltimateChargeDuration();
	//Enable Charging GO with particles
	//keep player still while charging
	mPlayerController->EnableChargeUltimate(true);
	mPlayerController->SetMovementSpeed(0.01f);
}

void UltimateChargeState::Exit()
{
	mPlayerController->EnableChargeUltimate(false);
	mPlayerController->SetMovementSpeed(1/0.01f);
}

StateType UltimateChargeState::GetType()
{
    return StateType::ULTIMATE_CHARGE;
}

bool UltimateChargeState::IsReady()
{
    return false;
}
