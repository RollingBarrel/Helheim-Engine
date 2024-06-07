#include "IdleState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"

IdleState::IdleState(PlayerController* player) : State(player)
{
}

IdleState::~IdleState()
{
}

StateType IdleState::HandleInput()
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

void IdleState::Update()
{
}

void IdleState::Enter()
{
    mPlayerController->SetAnimation("tIdle", 0.1f);
}

void IdleState::Exit()
{
}

StateType IdleState::GetType()
{
    return StateType::IDLE;
}
