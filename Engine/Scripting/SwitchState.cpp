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
	//TODO: Should not be immediate
	return StateType::AIM;
}

void SwitchState::Update()
{

}

void SwitchState::Enter()
{
	//mPlayerController->SetSpineAnimation("tSwitch", 0.1f);
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

void SwitchState::PlayAudio() 
{
}
