#include "ReloadState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"

ReloadState::ReloadState(PlayerController* player) : State(player)
{
}

ReloadState::~ReloadState()
{
}

StateType ReloadState::HandleInput()
{
	if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;

    //mSpecialAttackTimer += App->GetDt(); THIS TIMER SHOULD BE COORDINATED WITH THE ONE ON AIM STATE
    if (/*mPlayerController->GetWeapon()->GetAttackTime() < mSpecialAttackTimer &&
        (*/App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN ||
            App->GetInput()->GetGameControllerTrigger(RIGHT_TRIGGER) == ButtonState::BUTTON_DOWN)//)
    {
        //mSpecialAttackTimer = 0.0f;
        return StateType::ATTACK;
    }

    //mSpecialAttackTimer += App->GetDt(); THIS TIMER SHOULD BE COORDINATED WITH THE ONE ON AIM STATE
    if (/*mPlayerController->GetSpecialAttackCooldown() < mSpecialAttackTimer && mPlayerController->GetEnergyType() != EnergyType::NONE &&
        (*/App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN ||
            App->GetInput()->GetGameControllerTrigger(LEFT_TRIGGER) == ButtonState::BUTTON_DOWN)//)
    {
        //mSpecialAttackTimer = 0.0f;
        return StateType::SPECIAL;
    }

    //mSwitchTimer += App->GetDt(); THIS TIMER SHOULD BE COORDINATED WITH THE ONE ON AIM STATE
    if (/*mPlayerController->GetSwitchCooldown() < mSwitchTimer &&
        (*/App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN ||
            App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_Y) == ButtonState::BUTTON_DOWN)//)
    {
        //mSwitchTimer = 0.0f;
        return StateType::SWITCH;
    }

    //mGrenadeTimer += App->GetDt(); THIS TIMER SHOULD BE COORDINATED WITH THE ONE ON AIM STATE
    if (/*mPlayerController->GetGrenadeCooldown() < mGrenadeTimer &&
        (*/App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN ||
            App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_DOWN)//)
    {
        //mGrenadeTimer = 0.0f;
        return StateType::GRENADE;
    }

	mReloadTimer += App->GetDt();
	if (mReloadTimer > mPlayerController->GetReloadDuration())
	{
		return StateType::RELOAD;
	}
		
	return StateType::AIM;
}

void ReloadState::Update()
{
	mPlayerController->Reload();
}

void ReloadState::Enter()
{
	mReloadTimer = 0.0f;

	mPlayerController->SetSpineAnimation("tReload", 0.1f);
}

void ReloadState::Exit()
{
}

StateType ReloadState::GetType()
{
	return StateType::RELOAD;
}