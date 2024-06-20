#include "SwitchState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "GameManager.h"
#include "HudController.h"
#include "PlayerController.h"
#include "Weapon.h"

SwitchState::SwitchState(PlayerController* player) : State(player), mPlayerController(player)
{
}

SwitchState::~SwitchState()
{
}

StateType SwitchState::HandleInput()
{
	return StateType::AIM;

	//return State::HandleInput();
}

void SwitchState::Update()
{
}

void SwitchState::Enter()
{
    mPlayerController->SwitchWeapon();

	GameManager::GetInstance()->GetHud()->SwitchWeapon();
	mPlayerController->SwitchWeapon();
}

void SwitchState::Exit()
{
}

StateType SwitchState::GetType()
{
	return StateType::SWITCH;
}
