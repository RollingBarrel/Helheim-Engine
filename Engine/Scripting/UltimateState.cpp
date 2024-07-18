#include "UltimateState.h"
#include "UltimateAttack.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "GameObject.h"
#include "ScriptComponent.h"



UltimateState::UltimateState(PlayerController* player, float cooldown, float duration) : State(player, cooldown)
{
	mUltimateDuration = duration;
	GameObject* ultimateGO = mPlayerController->GetUltimateGO();
	if (ultimateGO)
	{
		mUltimateScript =  reinterpret_cast<UltimateAttack*>(reinterpret_cast<ScriptComponent*>(ultimateGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
	}
}

UltimateState::~UltimateState()
{
}

StateType UltimateState::HandleInput()
{
	//Keep returning ultimate until timer reaches end 
	if (!mTimer.Delay(mUltimateDuration))
		return StateType::ULTIMATE;
	else 
		return StateType::AIM;
}

void UltimateState::Update()
{
}

void UltimateState::Enter()
{
	mUltimateScript->mDamageTick = mPlayerController->GetUltimateDamageTick();
	mUltimateScript->mInterval = mPlayerController->GetUltimateDamageInterval();
	mUltimateDuration = mPlayerController->GetUltimateDuration();
	//mPlayerController->SetUltimateResource(0);
	mPlayerController->EnableUltimate(true);
	mPlayerController->SetMovementSpeed(mPlayerController->GetUltimateSlow());
}

void UltimateState::Exit()
{
	mPlayerController->EnableUltimate(false);
	mPlayerController->SetMovementSpeed(1.0f/ mPlayerController->GetUltimateSlow());
	SetCooldown(mPlayerController->GetUltimateCooldown());
}

StateType UltimateState::GetType()
{
	return StateType::ULTIMATE;
}

bool UltimateState::IsReady()
{
	mStateTimer += App->GetDt();
	if (mPlayerController->GetUltimateGO() && mStateTimer >= mStateCooldown ) return true;
	return false;
}

