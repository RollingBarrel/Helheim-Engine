#include "MoveState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "GameObject.h"
#include "Keys.h"
#include "PlayerController.h"
#include "GameManager.h"

MoveState::MoveState(PlayerController* player) : State(player)
{
    mMoveDirection = float3::zero;
    mCameraFront = App->GetCamera()->GetCurrentCamera()->GetOwner()->GetRight().Cross(float3::unitY).Normalized();
}

MoveState::~MoveState()
{
}

StateType MoveState::HandleInput()
{
    // Check dash cooldown
    mDashTimer += App->GetDt();
    if (mDashTimer > mPlayerController->GetDashCoolDown() &&
        App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_DOWN)
    {
        mDashTimer = 0.0f;
        return StateType::DASH;
    }
    else if (GameManager::GetInstance()->UsingController())
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

void MoveState::Update()
{
    mMoveDirection = float3::zero;

    if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT)
    {
        mMoveDirection += mCameraFront;
    }
    else if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) > 0)
    {
        mMoveDirection -= mCameraFront * App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
    }

    if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT)
    {
        mMoveDirection -= mCameraFront;
    }
    else if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) < 0)
    {
        mMoveDirection -= mCameraFront * App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
    }

    if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT)
    {
        mMoveDirection += float3::unitY.Cross(mCameraFront);
    }
    else if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) > 0)
    {
        mMoveDirection -= float3::unitY.Cross(mCameraFront) * App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
    }

    if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
    {
        mMoveDirection -= float3::unitY.Cross(mCameraFront);
    }
    else if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) < 0)
    {
        mMoveDirection -= float3::unitY.Cross(mCameraFront) * App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
    }

    mPlayerController->MoveInDirection(mMoveDirection);

    DoAnimation();
    DoAudio();
}

void MoveState::Enter()
{
}

void MoveState::Exit()
{
}

StateType MoveState::GetType()
{
    return StateType::MOVE;
}

void MoveState::DoAnimation() 
{
    if (!mMoveDirection.Equals(float3::zero)) 
    {
        mMoveDirection.Normalize();
        float2 mMovingTo = SetMovingDirection();
        float3 mousePosition = mPlayerController->GetPlayerAimPosition();

        std::string animation;

        auto setAnimation = [&](const std::string& forward, const std::string& back, const std::string& left, const std::string& right) 
            {
            if (mMovingTo.x == 0 && mMovingTo.y == 1) animation = forward;
            else if (mMovingTo.x == 0 && mMovingTo.y == -1) animation = back;
            else if (mMovingTo.x == 1 && mMovingTo.y == 0) animation = right;
            else if (mMovingTo.x == -1 && mMovingTo.y == 0) animation = left;
            else if (mMovingTo.x == 1 && mMovingTo.y == 1) animation = forward;
            else if (mMovingTo.x == -1 && mMovingTo.y == 1) animation = forward;
            else if (mMovingTo.x == 1 && mMovingTo.y == -1) animation = back;
            else if (mMovingTo.x == -1 && mMovingTo.y == -1) animation = back;
            };

        if (mousePosition.z > 0) 
        { // Looking UP
            setAnimation("tWalkForward", "tWalkBack", "tStrafeLeft", "tStrafeRight");
        }
        else if (mousePosition.z < 0) 
        { // Looking DOWN
            setAnimation("tWalkBack", "tWalkForward", "tStrafeRight", "tStrafeLeft");
        }
        else if (mousePosition.x < 0) 
        { // Looking LEFT
            setAnimation("tStrafeRight", "tStrafeLeft", "tWalkForward", "tWalkBack");
        }
        else if (mousePosition.x > 0) 
        { // Looking RIGHT
            setAnimation("tStrafeLeft", "tStrafeRight", "tWalkBack", "tWalkForward");
        }

        mPlayerController->SetAnimation(animation, 0.3f);
    }
}

float2 MoveState::SetMovingDirection() 
{
    float absX = std::abs(mMoveDirection.x);
    float absZ = std::abs(mMoveDirection.z);

    if (mMoveDirection.x > 0 && mMoveDirection.z > 0) 
    {
        return (absX > absZ) ? float2(1, 0) : float2(0, 1); // Moving right or up
    }
    else if (mMoveDirection.x > 0 && mMoveDirection.z < 0) 
    {
        return (absX > absZ) ? float2(1, 0) : float2(0, -1); // Moving right or down
    }
    else if (mMoveDirection.x < 0 && mMoveDirection.z > 0) 
    {
        return (absX > absZ) ? float2(-1, 0) : float2(0, 1); // Moving left or up
    }
    else if (mMoveDirection.x < 0 && mMoveDirection.z < 0) 
    {
        return (absX > absZ) ? float2(-1, 0) : float2(0, -1); // Moving left or down
    }
    else if (mMoveDirection.x == 0 && mMoveDirection.z != 0) 
    {
        return (mMoveDirection.z > 0) ? float2(0, 1) : float2(0, -1); // Moving up or down
    }
    else if (mMoveDirection.z == 0 && mMoveDirection.x != 0) 
    {
        return (mMoveDirection.x > 0) ? float2(1, 0) : float2(-1, 0); // Moving right or left
    }
    else 
    {
        return float2(0, 0); // Not moving
    }
}

void MoveState::DoAudio()
{
    // TODO: play sound according the animation
    mStepTimer += App->GetDt();
    if (mStepTimer >= mStepCooldown) 
    {
        mStepTimer = 0;
        mPlayerController->PlayOneShot("Step");
    }
}
