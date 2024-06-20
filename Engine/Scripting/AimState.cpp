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
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;

    mGrenadeTimer += App->GetDt();
    if (mPlayerController->GetGrenadeCooldown() < mGrenadeTimer &&
       (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN || 
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_DOWN))
    {
        mGrenadeTimer = 0.0f;
        return StateType::GRENADE;
    }

    mSpecialAttackTimer += App->GetDt();
    if (mPlayerController->GetWeapon()->GetAttackTime() < mSpecialAttackTimer &&
       (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerTrigger(RIGHT_TRIGGER) == ButtonState::BUTTON_DOWN))
    {
        mSpecialAttackTimer = 0.0f;
        return StateType::ATTACK;
    }

    mSpecialAttackTimer += App->GetDt();
    if (mPlayerController->GetSpecialAttackCooldown() < mSpecialAttackTimer && mPlayerController->GetEnergyType() != EnergyType::NONE &&
       (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerTrigger(LEFT_TRIGGER) == ButtonState::BUTTON_DOWN))
    {
        mSpecialAttackTimer = 0.0f;
        return StateType::SPECIAL;
    }

    mSwitchTimer += App->GetDt();
    if (mPlayerController->GetSwitchCooldown() < mSwitchTimer &&
       (App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_Y) == ButtonState::BUTTON_DOWN))
    {
        mSwitchTimer = 0.0f;
        return StateType::SWITCH;
    }

    if (mPlayerController->CanReload() &&
       (App->GetInput()->GetKey(Keys::Keys_R) == KeyState::KEY_DOWN ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_X) == ButtonState::BUTTON_DOWN))
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