#include "DashState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "float3.h"
#include "PlayerController.h"
#include "GameManager.h"

#include "ScriptComponent.h"
#include "PoolManager.h"
#include "TrailComponent.h"
#include "GameObject.h"

DashState::DashState(PlayerController* player, float cooldown) : State(player, cooldown)
{
    mDashVFX = mPlayerController->GetDashVFX();
    if (mDashVFX)
    {
        mDashTrail = reinterpret_cast<TrailComponent*>(mDashVFX->GetComponent(ComponentType::TRAIL));
    }
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
        if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != 0 ||
            App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != 0)
        {
            return StateType::MOVE;
        }
    }
    else
    {
        if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT ||
            App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
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
    float3 direction;

    float collisionDotProduct = mPlayerController->GetPlayerDirection().Dot(mPlayerController->GetCollisionDirection());
    if (collisionDotProduct < 0.0f)
    {
        direction = mPlayerController->GetPlayerDirection() - mPlayerController->GetCollisionDirection().Mul(collisionDotProduct);
    }
    else
    {
        direction = mPlayerController->GetPlayerDirection();
    }

    float3 futurePos = currentPos + (direction) * dashSpeed * App->GetDt();

    mPlayerController->MoveToPosition(futurePos);

    //Dash VFX
    ColorGradient gradient;
    gradient.AddColorGradientMark(0.1f, float4(1.0f, 0.0f, 0.1f, 1.0f));

    if (mDashTrail)
    {
        mDashTrail->SetEnable(true);
        if (&gradient)
        {
            mDashTrail->SetColorGradient(gradient);
        }
    }
}

void DashState::Enter()
{
    mDashTimer = 0.0f;

    mDashVFX->SetEnabled(true);
    mInitialPos = mPlayerController->GetPlayerPosition();
}

void DashState::Exit()
{
    mDashTrail->SetEnable(false);
}

StateType DashState::GetType()
{
    return StateType::DASH;
}