#include "IdleState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"

IdleState::IdleState(PlayerController* player) : State(player)
{
}

IdleState::~IdleState()
{
}

StateType IdleState::HandleInput()
{
    // Check dash cooldown
    if (App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_DOWN)
    {
        return StateType::DASH;
    }
    else if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {

        return StateType::MOVE;
    }

    return State::HandleInput();
}

void IdleState::Update()
{
}

void IdleState::Enter()
{
}

void IdleState::Exit()
{
}
