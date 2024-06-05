#include "GrenadeState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"

GrenadeState::GrenadeState(PlayerController* player) : State(player)
{
}

GrenadeState::~GrenadeState()
{
}

StateType GrenadeState::HandleInput()
{
    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP)
    {
        return StateType::AIM;
    }

	return State::HandleInput();
}

void GrenadeState::Update()
{
}

void GrenadeState::Enter()
{
}

void GrenadeState::Exit()
{
}
