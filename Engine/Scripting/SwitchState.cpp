#include "SwitchState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "GameManager.h"
#include "HudController.h"
#include "PlayerController.h"
#include "Weapon.h"

SwitchState::SwitchState(PlayerController* player, float cooldown) : State(player, cooldown)
{
}

SwitchState::~SwitchState()
{
}

StateType SwitchState::HandleInput()
{
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;

    mSwitchTimer += App->GetDt();
    if (mSwitchTimer < mPlayerController->GetSwitchDuration())
    {
        
        return StateType::SWITCH;
    }

    return StateType::AIM;
}

void SwitchState::Update()
{
}

void SwitchState::Enter()
{
    mSwitchTimer = 0.0f;

    if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE) {
        mPlayerController->SetSpineAnimation("tSwitch_To_Melee", 0.3f);
        mPlayerController->EquipMeleeWeapon(true);
    }else 
    {
        mPlayerController->SetSpineAnimation("tSwitch_To_Ranged", 0.3f);
        mPlayerController->EquipMeleeWeapon(false);
    }
        

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
