#include "SwitchState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "GameManager.h"
#include "HudController.h"

SwitchState::SwitchState(PlayerController* player) : State(player)
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
	GameManager::GetInstance()->GetHud()->SwitchWeapon();
}

void SwitchState::Enter()
{
}

void SwitchState::Exit()
{
}
