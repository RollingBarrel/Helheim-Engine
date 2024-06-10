#include "DashState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "float3.h"
#include "PlayerController.h"
#include "GameManager.h"

DashState::DashState(PlayerController* player) : State(player)
{
}

DashState::~DashState()
{
}

StateType DashState::HandleInput()
{
    mDashTimer += App->GetDt();
    if (mDashTimer <= mPlayerController->GetDashDuration()) return StateType::DASH;

    if (GameManager::GetInstance()->UsingController())
    {
        if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
        {
            return StateType::MOVE;
        }
    }
    else
    {
        if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != 0 ||
            App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != 0 ||
            App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTX) != 0 ||
            App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_RIGHTY) != 0)
        {
            return StateType::MOVE;
        }
    }

    return StateType::IDLE;
}

void DashState::Update()
{
    float dashSpeed = mPlayerController->GetDashRange() / mPlayerController->GetDashDuration();
    float3 currentPos = mPlayerController->GetPlayerPosition();
    float3 futurePos = currentPos + mPlayerController->GetPlayerDirection() * dashSpeed * App->GetDt();

    mPlayerController->MoveToPosition(futurePos);
}

void DashState::Enter()
{
    mDashTimer = 0.0f;
}

void DashState::Exit()
{
}

StateType DashState::GetType()
{
    return StateType::DASH;
}
