#include "DashState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"

DashState::DashState(PlayerController* player) : State(player)
{
}

DashState::~DashState()
{
}

StateType DashState::HandleInput()
{
    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
        App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {

        return StateType::MOVE;
    }

    return StateType::IDLE;
}

void DashState::Update()
{
}

void DashState::Enter()
{
}

void DashState::Exit()
{
}
