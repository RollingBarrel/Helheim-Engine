#include "SwitchState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "GameManager.h"
#include "HudController.h"
#include "PlayerController.h"

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
	mPlayerController->SwitchWeapon();
	GameManager::GetInstance()->GetHud()->SwitchWeapon();
}

void SwitchState::Enter()
{
}

void SwitchState::Exit()
{
}

StateType SwitchState::GetType()
{
	return StateType::SWITCH;
}

void SwitchState::PlayAudio() 
{
}
