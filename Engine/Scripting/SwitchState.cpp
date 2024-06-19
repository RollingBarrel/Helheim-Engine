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
    if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE)
    {
        mPlayerController->SwitchWeapon(mPlayerController->GetMeleeWeapon());
    }
    else
    {
        mPlayerController->SwitchWeapon(mPlayerController->GetRangeWeapon());
    }

	GameManager::GetInstance()->GetHud()->SwitchWeapon();
}

void SwitchState::Exit()
{
}

StateType SwitchState::GetType()
{
	return StateType::SWITCH;
}
