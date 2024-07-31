#include "UltimateState.h"
#include "UltimateAttack.h"
#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "GameObject.h"
#include "ScriptComponent.h"
#include "GameManager.h"
#include "HudController.h"

UltimateState::UltimateState(PlayerController* player, float cooldown, float duration) : State(player, cooldown)
{
	mUltimateDuration = duration;
	GameObject* ultimateGO = mPlayerController->GetUltimateGO();
	if (ultimateGO)
	{
		mUltimateScript = static_cast<UltimateAttack*>(static_cast<ScriptComponent*>(ultimateGO->GetComponent(ComponentType::SCRIPT))->GetScriptInstance());
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

	if (GameManager::GetInstance()->GetHud())
	{
		GameManager::GetInstance()->GetHud()->SetUltimateCooldown(mPlayerController->GetUltimateCooldown());
	}
}

StateType UltimateState::GetType()
{
	return StateType::ULTIMATE;
}

bool UltimateState::IsReady()
{
	if (mPlayerController->GetUltimateGO() && mStateTimer.DelayWithoutReset(mStateCooldown)) return true;
	return false;
}

