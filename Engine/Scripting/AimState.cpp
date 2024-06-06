#include "AimState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"

AimState::AimState(PlayerController* player) : State(player)
{
}

AimState::~AimState()
{
}

StateType AimState::HandleInput()
{
    mGrenadeTimer += App->GetDt();
    if (mPlayerController->GetGrenadeCooldown() >= mGrenadeTimer &&
        App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN)
    {
        return StateType::GRENADE;
    }

    mAttackTimer += App->GetDt();
    if (mPlayerController->GetAttackCooldown() >= mAttackTimer &&
        App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN)
    {
        return StateType::ATTACK;
    }

    mSpecialAttackTimer += App->GetDt();
    if (mPlayerController->GetSpecialAttackCooldown() >= mSpecialAttackTimer &&
        App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
    {
        return StateType::SPECIAL;
    }

    mSwitchTimer += App->GetDt();
    if (mPlayerController->GetSwitchCooldown() >= mSwitchTimer &&
        App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN)
    {
        return StateType::SWITCH;
    }

    if (mPlayerController->CanReload() &&
        App->GetInput()->GetKey(Keys::Keys_R) == KeyState::KEY_DOWN)
    {
        return StateType::RELOAD;
    }

    
	return State::HandleInput();
}

void AimState::Update()
{
}

void AimState::Enter()
{
}

void AimState::Exit()
{
}
