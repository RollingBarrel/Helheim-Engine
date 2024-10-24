#include "MoveState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "GameObject.h"
#include "Keys.h"
#include "Weapon.h"
#include "PlayerController.h"
#include "GameManager.h"
#include "DashState.h"
#include "MathFunc.h"

MoveState::MoveState(PlayerController* player, float cooldown) : State(player, cooldown)
{
	mMoveDirection = float3::zero;
	mCameraFront = App->GetCamera()->GetCurrentCamera()->GetOwner()->GetRight().Cross(float3::unitY).Normalized();
}

MoveState::~MoveState()
{
}

StateType MoveState::HandleInput()
{

	if (mPlayerController->GetDashState()->IsReady())
	{
		mPlayerController->GetDashState()->ResetCooldown();
		return StateType::DASH;
	}

	if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != 0 ||
		App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != 0 ||
		App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT ||
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
	mMoveDirection = float3::zero;
	mCameraFront = App->GetCamera()->GetCurrentCamera()->GetOwner()->GetRight().Cross(float3::unitY).Normalized(); //TODO: Let this only in start when transforms fixed
	if (GameManager::GetInstance()->UsingController())
	{
		if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY) != 0)
		{
			mMoveDirection -= mCameraFront * App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTY);
		}

		if (App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX) != 0)
		{
			mMoveDirection -= float3::unitY.Cross(mCameraFront) * App->GetInput()->GetGameControllerAxisValue(ControllerAxis::SDL_CONTROLLER_AXIS_LEFTX);
		}
	}
	else
	{
		if (App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_W) == KeyState::KEY_REPEAT)
		{
			mMoveDirection += mCameraFront;
		}

		if (App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_S) == KeyState::KEY_REPEAT)
		{
			mMoveDirection -= mCameraFront;
		}

		if (App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_A) == KeyState::KEY_REPEAT)
		{
			mMoveDirection += float3::unitY.Cross(mCameraFront);
		}

		if (App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_DOWN || App->GetInput()->GetKey(Keys::Keys_D) == KeyState::KEY_REPEAT)
		{
			mMoveDirection -= float3::unitY.Cross(mCameraFront);
		}
	}

	// Early return if there's no move direction vector
	if (mMoveDirection.Equals(float3::zero)) return;

	mMoveDirection.Normalize();
	mPlayerController->MoveInDirection(mMoveDirection);

	DoAnimation();
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
		PlayAudio();
		mMoveDirection.Normalize();
		float2 mMovingTo = SetMovingDirection();
		float3 mousePosition = GameManager::GetInstance()->GetPlayer()->GetFront();
		
		/*OUTDATED
		//LOG("x:%f , y:%f", mMovingTo.x, mMovingTo.y);
		std::string animation;

		auto setAnimation = [&](const std::string& up, const std::string& down, const std::string& left, const std::string& right)
			{
				if (mMovingTo.x == 0 && mMovingTo.y == 1) { animation = up; }
				else if (mMovingTo.x == 0 && mMovingTo.y == -1) { animation = down; }
				else if (mMovingTo.x == 1 && mMovingTo.y == 0) { animation = right; }
				else if (mMovingTo.x == -1 && mMovingTo.y == 0) { animation = left; }
				else if (mMovingTo.x == 1 && mMovingTo.y == 1) { animation = up; }
				else if (mMovingTo.x == -1 && mMovingTo.y == 1) { animation = up; }
				else if (mMovingTo.x == 1 && mMovingTo.y == -1) { animation = down; }
				else if (mMovingTo.x == -1 && mMovingTo.y == -1) { animation = down; }
			};

		if (mousePosition.z > 0 && mousePosition.x < 0)
		{ // Looking UP
			setAnimation("tWalkForward", "tWalkBack", "tStrafeLeft", "tStrafeRight");
		}
		else if (mousePosition.z < 0 && mousePosition.x > 0)
		{ // Looking DOWN
			setAnimation("tWalkBack", "tWalkForward", "tStrafeRight", "tStrafeLeft");
		}
		else if (mousePosition.x > 0 && mousePosition.z > 0)
		{ // Looking LEFT
			setAnimation("tStrafeRight", "tStrafeLeft", "tWalkForward", "tWalkBack");
		}
		else if (mousePosition.x < 0 && mousePosition.z < 0)
		{ // Looking RIGHT
			setAnimation("tStrafeLeft", "tStrafeRight", "tWalkBack", "tWalkForward");
		}
		//LOG("x:%f ", animation);
		*/
		std::string animation = GetTriggerFromAngle(ComputeMoveAnge(mousePosition));
		mPlayerController->SetAnimation(animation, 0.01f);
		StateType upperBodyState = mPlayerController->GetPlayerUpperState()->GetType();
		if (upperBodyState == StateType::ULTIMATE_CHARGE)
		{
			mPlayerController->SetLowerAnimationSpeed(0.6f); // TODO: Modify this with a script value?

		}
		else
		{
			mPlayerController->SetLowerAnimationSpeed(2.0f); // TODO: Modify this with a script value?

		}

	}
}

float2 MoveState::SetMovingDirection()
{
	float absX = std::abs(mMoveDirection.x);
	float absZ = std::abs(mMoveDirection.z);

	//LOG("x:%f , z:%f", mMoveDirection.x, mMoveDirection.z)

	if (mMoveDirection.x > 0 && mMoveDirection.z > 0 && absZ > absX)
	{
		return float2(-1, 0);
	}
	else if (mMoveDirection.x > 0 && mMoveDirection.z < 0 && absX > absZ)
	{
		return float2(0, -1);
	}
	else if (mMoveDirection.x < 0 && mMoveDirection.z > 0 && absX > absZ)
	{
		return float2(0, 1);
	}
	else if (mMoveDirection.x < 0 && mMoveDirection.z < 0 && absZ > absX)
	{
		return float2(1, 0);
	}
	else if (mMoveDirection.x < 0 && mMoveDirection.z < 0 && absX > absZ)
	{
		return float2(1, 1);
	}
	else if (mMoveDirection.x < 0 && mMoveDirection.z > 0 && absZ > absX)
	{
		return float2(-1, 1);
	}
	else if (mMoveDirection.x > 0 && mMoveDirection.z < 0 && absZ > absX)
	{
		return float2(1, -1);
	}
	else if (mMoveDirection.x > 0 && mMoveDirection.z > 0 && absX > absZ)
	{
		return float2(-1, -1);
	}
	else
	{
		return float2(0, 0); // Not moving
	}
}

float MoveState::ComputeMoveAnge(float3 mouseDir)
{
	// Step 1: Project the vectors onto the Y plane (XZ plane)
	float2 frontProjected(mouseDir.x, mouseDir.z);       // Projected front (XZ plane)
	float2 directionProjected(mMoveDirection.x, mMoveDirection.z); // Projected direction (XZ plane)


	// Step 2: Normalize the projected 2D vectors
	frontProjected.Normalize();
	directionProjected.Normalize();

	// Step 3: Compute the dot product to get the cosine of the angle
	float dotProduct = frontProjected.Dot(directionProjected);

	// Clamp the dot product to avoid precision errors in acos
	dotProduct = Clamp(dotProduct, -1.0f, 1.0f);

	// Step 4: Compute the angle in radians using acos (dot product gives cos of angle)
	float angleRadians = Acos(dotProduct) ;

	float crossProductZ = frontProjected.x * directionProjected.y - frontProjected.y * directionProjected.x;

	// If the cross product is negative, the angle is clockwise, so we make it negative
	if (crossProductZ < 0) 
	{
		angleRadians = -angleRadians;
	}

	// Convert the angle to degrees
	return RadToDeg(angleRadians);

}

std::string MoveState::GetTriggerFromAngle(float angle)
{
	// Normalize the angle to the range [-180, 180)
	if (angle > 180.0f) 
	{
		angle -= 360.0f;
	}
	else if (angle <= -180.0f) 
	{
		angle += 360.0f;
	}

	// Array of strings representing the actions for each octet
	std::vector<std::string> octetStrings = 
	{
		"tWalkForward",			// -22.5 to 22.5 degrees
		"tWalkFrontRight",		// 22.5 to 67.5 degrees
		"tStrafeRight",         // 67.5 to 112.5 degrees
		"tWalkBackRight",		// 112.5 to 157.5 degrees
		"tWalkBack",			// 157.5 to -157.5 degrees
		"tWalkBackLeft",		// -157.5 to -112.5 degrees
		"tStrafeLeft",          // -112.5 to -67.5 degrees
		"tWalkFrontLeft"		// -67.5 to -22.5 degrees
	};

	// Compute which octet the angle belongs to
	if (angle > -22.5f && angle <= 22.5f) 
	{
		return octetStrings[0]; // tWalkForward
	}
	else if (angle > 22.5f && angle <= 67.5f) 
	{
		return octetStrings[1]; // tWalkForwardRight
	}
	else if (angle > 67.5f && angle <= 112.5f) 
	{
		return octetStrings[2]; // tWalkRight
	}
	else if (angle > 112.5f && angle <= 157.5f) 
	{
		return octetStrings[3]; // tWalkBackwardRight
	}
	else if (angle > 157.5f || angle <= -157.5f) 
	{
		return octetStrings[4]; // tWalkBackward
	}
	else if (angle > -157.5f && angle <= -112.5f) 
	{
		return octetStrings[5]; // tWalkBackwardLeft
	}
	else if (angle > -112.5f && angle <= -67.5f) 
	{
		return octetStrings[6]; // tWalkLeft
	}
	else 
	{ // angleDegrees > -67.5f && angleDegrees <= -22.5f
		return octetStrings[7]; // tWalkForwardLeft
	}
}

void MoveState::PlayAudio()
{
	if (mStepTimer.Delay(mStepCooldown))
	{
		GameManager::GetInstance()->PlayPlayerFootStepSound();
	}
}
