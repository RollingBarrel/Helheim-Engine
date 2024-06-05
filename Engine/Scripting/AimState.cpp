#include "AimState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"

AimState::AimState(PlayerController* player) : State(player)
{
}

AimState::~AimState()
{
}

StateType AimState::HandleInput()
{
    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_LEFT) == KeyState::KEY_DOWN)
    {
        return StateType::SPECIAL;
    }

    if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN)
    {
        return StateType::ATTACK;
    }

    if (App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN)
    {
        return StateType::SWITCH;
    }

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN)
    {
        return StateType::GRENADE;
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
