#include "DashState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleDetourNavigation.h"
#include "Keys.h"
#include "float3.h"
#include "PlayerController.h"
#include "GameManager.h"
#include "AudioManager.h"

#include "ScriptComponent.h"
#include "PoolManager.h"
#include "TrailComponent.h"
#include "ParticleSystemComponent.h"
#include "GameObject.h"
#include "Trail.h"

DashState::DashState(PlayerController* player, float cooldown) : State(player, cooldown)
{
	mDashVFX = mPlayerController->GetDashVFX();
	if (mDashVFX)
	{
		mDashVFX->SetEnabled(true);

		if (mDashVFX->GetChildren().size() == 3)
		{
			mDashParticles = mDashVFX->GetChildren()[0];
			mBegginingParticles = mDashVFX->GetChildren()[1];
			mEndingParticles = mDashVFX->GetChildren()[2];
		}

		if (mDashParticles)
		{
			mDashParticles->SetEnabled(false);
			reinterpret_cast<ParticleSystemComponent*>(mDashParticles->GetComponent(ComponentType::PARTICLESYSTEM))
				->SetDuration(mPlayerController->GetDashDuration());
		}
		if (mBegginingParticles)
		{
			mBegginingParticles->SetEnabled(false);
			//Duration set from the inspector (the time should be low)
		}
		if (mEndingParticles)
		{
			mEndingParticles->SetEnabled(false);
			//Duration set from the inspector
		}
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
	if (GameManager::GetInstance()->IsPaused()) return;

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

	float3 futurePos = currentPos + (direction)*dashSpeed * App->GetDt();

	mPlayerController->MoveToPosition(futurePos);
}

void DashState::Enter()
{
	mDashTimer = 0.0f;
	GameManager::GetInstance()->GetAudio()->PlayOneShot(SFX::PLAYER_DASH, GameManager::GetInstance()->GetPlayer()->GetWorldPosition());


	if (mDashParticles)
	{
		mDashParticles->SetEnabled(false);
		mDashParticles->SetEnabled(true);
	}
	if (mBegginingParticles)
	{
		mBegginingParticles->SetEnabled(false);
		mBegginingParticles->SetEnabled(true);
	}

	//Pause Animation -> Set animation time -> Pause player rotation(Already done in PlayerController::HandleRotation() -> rotate player in dash direction
	//Option 2: Pause Animation -> Set animation time (look at current animation from lower state machine 8 move states)
	
	mPlayerController->SetIsAnimationPlaying(false);
	/*
	std::string moveDir = mPlayerController->GetLowerAnimState();

	std::vector<std::string> octetStrings =
	{
		"WalkForward",
		"WalkFrontRight",
		"StrafeRight",
		"WalkBackRight",
		"WalkBack",
		"WalkBackLeft",
		"StrafeLeft",
		"WalkFrontLeft"
	};
	std::vector<float> octetTimes =
	{
		17.5f,
		19.95f,
		20.85f,
		24.1f,
		19.15f,
		24.9f,
		20.05f,
		23.05f
	};

	float animTime = 17.5f; // default if animation is not movement
	for (unsigned int i = 0; i<octetStrings.size(); ++i)
	{
		if (octetStrings[i].compare(moveDir))
		{
			animTime = octetTimes[i];
		}
	}


	mPlayerController->SetAnimationTime(animTime);
	*/
	//mPlayerController->DashLookAtFront();
	
}

void DashState::Exit()
{
	if (mEndingParticles)
	{
		mEndingParticles->SetEnabled(false);
		mEndingParticles->SetEnabled(true);
	}
	//Unpause Animation -> Restart animation state -> Unpause player rotation
	mPlayerController->SetIsAnimationPlaying(true);
	//mPlayerController->RestartAnimationState();
}

bool DashState::IsReady()
{
	if (mStateTimer.DelayWithoutReset(mStateCooldown) &&
		(App->GetInput()->GetKey(Keys::Keys_SPACE) == KeyState::KEY_DOWN ||
			App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER) == ButtonState::BUTTON_DOWN))
	{
		return true;
	}

	return false;
}

StateType DashState::GetType()
{
	return StateType::DASH;
}