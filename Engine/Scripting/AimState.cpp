#include "AimState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Weapon.h"

AimState::AimState(PlayerController* player) : State(player)
{
}

AimState::~AimState()
{
}

StateType AimState::HandleInput()
{
    /*Weapon* weapon = mPlayerController->GetWeapon();
    if (weapon->GetType() == Weapon::WeaponType::RANGE && weapon->GetCurrentAmmo() == 0)
    {
        return StateType::RELOAD;
    }*/

    mGrenadeTimer += App->GetDt();
    if (mPlayerController->GetGrenadeCooldown() < mGrenadeTimer &&
        App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN)
    {
        mGrenadeTimer = 0;
        return StateType::GRENADE;
    }

    //mAttackTimer += App->GetDt();
    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
    {
        return StateType::ATTACK;
    }

    mSpecialAttackTimer += App->GetDt();
    if (mPlayerController->GetSpecialAttackCooldown() < mSpecialAttackTimer &&
        App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN &&
        mPlayerController->GetBatteryType() != BatteryType::NONE)
    {
        mSpecialAttackTimer = 0;
        return StateType::SPECIAL;
    }

    mSwitchTimer += App->GetDt();
    if (mPlayerController->GetSwitchCooldown() < mSwitchTimer &&
        App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN)
    {
        mSwitchTimer = 0;
        return StateType::SWITCH;
    }

    if (mPlayerController->CanReload() &&
        App->GetInput()->GetKey(Keys::Keys_R) == KeyState::KEY_DOWN)
    {
        return StateType::RELOAD;
    }

	return StateType::AIM;
}

void AimState::Update()
{
}

void AimState::Enter()
{
    //It can't be done when entering because in that case the others animations would be one frame
    //Maybe with buffers
    mPlayerController->SetSpineAnimation("tAim", 0.1f);
}

void AimState::Exit()
{
}

StateType AimState::GetType()
{
    return StateType::AIM;
}

void AimState::PlayAudio()
{
}