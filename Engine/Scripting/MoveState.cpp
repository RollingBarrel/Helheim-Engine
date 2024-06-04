#include "MoveState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "GameObject.h"
#include "Keys.h"
#include "PlayerController.h"

MoveState::MoveState(PlayerController* player) : State(player)
{
}

MoveState::~MoveState()
{
    mCameraFront = App->GetCamera()->GetCurrentCamera()->GetOwner()->GetRight().Cross(float3::unitY).Normalized();
}

StateType MoveState::HandleInput()
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

	return StateType::IDLE;
}

void MoveState::Update()
{
    float3 moveDirection = float3::zero;

    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT)
    {
        moveDirection += mCameraFront;

    }

    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT)
    {
        moveDirection -= mCameraFront;
    }

    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT)
    {
        moveDirection += float3::unitY.Cross(mCameraFront);

    }

    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        moveDirection -= float3::unitY.Cross(mCameraFront);

    }

    mPlayerController->MoveInDirection(moveDirection);
}

void MoveState::Enter()
{
}

void MoveState::Exit()
{
}
