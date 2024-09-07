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
#include "ParticleSystemComponent.h"
#include "GameObject.h"
#include "Trail.h"

DashState::DashState(PlayerController* player, float cooldown) : State(player, cooldown)
{
    mDashVFX = mPlayerController->GetDashVFX();
    if (mDashVFX)
    {
        mDashVFX->SetEnabled(true);

        if (mDashVFX->GetChildren().size() == 4) 
        {
            mDashTrail = reinterpret_cast<TrailComponent*>(mDashVFX->GetComponentInChildren(ComponentType::TRAIL));
            mDashParticles = reinterpret_cast<ParticleSystemComponent*>(mDashVFX->GetChildren()[1]->GetComponent(ComponentType::PARTICLESYSTEM));
            mBegginingParticles = reinterpret_cast<ParticleSystemComponent*>(mDashVFX->GetChildren()[2]->GetComponent(ComponentType::PARTICLESYSTEM));
            mEndingParticles = reinterpret_cast<ParticleSystemComponent*>(mDashVFX->GetChildren()[3]->GetComponent(ComponentType::PARTICLESYSTEM));
        }
        
        if (mDashTrail)
        {
            mDashTrail->SetEnable(false);
        }
        if (mDashParticles)
        {
            mDashParticles->SetEnable(false);
            mDashParticles->SetLoop(false);
            mDashParticles->SetDuration(mPlayerController->GetDashDuration());
        }
        if (mBegginingParticles)
        {
            mBegginingParticles->SetEnable(false);
            mBegginingParticles->SetLoop(false);
            //Duration set from the inspector (the time should be low)
        }
        if (mEndingParticles)
        {
            mEndingParticles->SetEnable(false);
            mEndingParticles->SetLoop(false);
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

    float3 futurePos = currentPos + (direction) * dashSpeed * App->GetDt();

    mPlayerController->MoveToPosition(futurePos);
}

void DashState::Enter()
{
    mDashTimer = 0.0f;

    if (mDashTrail)
    {
        mDashTrail->SetEnable(false);
        mDashTrail->SetEnable(true);

        mDashTrail->GetTrail()->SetLifeTime(0.3f); //The value is changed manually here
    }
    if (mDashParticles)
    {
        mDashParticles->SetEnable(false);
        mDashParticles->SetEnable(true);
    }
    if (mBegginingParticles) 
    {
        mBegginingParticles->SetEnable(false);
        mBegginingParticles->SetEnable(true);
    }
}

void DashState::Exit()
{
    if (mDashTrail)
    {
        mDashTrail->GetTrail()->SetLifeTime(0.01f); //The lifetime is changed so something very small so it doesn't appear
    }

    if (mEndingParticles)
    {
        mEndingParticles->SetEnable(false);
        mEndingParticles->SetEnable(true);
    }
}

StateType DashState::GetType()
{
    return StateType::DASH;
}