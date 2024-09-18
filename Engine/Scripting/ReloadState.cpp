#include "ReloadState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "GrenadeState.h"
#include "AttackState.h"
#include "SpecialState.h"
#include "SwitchState.h"
#include "UltimateState.h"

ReloadState::ReloadState(PlayerController* player, float cooldown) : State(player, cooldown)
{
}

ReloadState::~ReloadState()
{
}

StateType ReloadState::HandleInput()
{
	if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;

    if (mPlayerController->GetGrenadeState()->IsReady())
    {
        mPlayerController->GetGrenadeState()->ResetCooldown();
        return StateType::GRENADE;
    }

    if (mPlayerController->GetAttackState()->IsReady())
    {
        return StateType::ATTACK;
    }

    if (mPlayerController->GetSpecialState()->IsReady())
    {
        mPlayerController->GetSpecialState()->ResetCooldown();
        return StateType::SPECIAL;
    }

    //if (mPlayerController->GetSwitchState()->IsReady() &&
    //    (App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN ||
    //        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_Y) == ButtonState::BUTTON_DOWN))
    //{
    //    mPlayerController->GetSwitchState()->ResetCooldown();
    //    return StateType::SWITCH;
    //}

	mReloadTimer += App->GetDt();
	if (mReloadTimer < mPlayerController->GetReloadDuration())
	{
		return StateType::RELOAD;
	}
    else
    {
        mDoRecharge = true;
    }
		
	return StateType::AIM;
}

void ReloadState::Update()
{
}

void ReloadState::Enter()
{
	mReloadTimer = 0.0f;
    mDoRecharge = false;

	mPlayerController->SetSpineAnimation("tReload", 0.1f);

    mPlayerController->EnableLaser(false);
}

void ReloadState::Exit()
{
    if(mDoRecharge)	mPlayerController->Reload();

    mPlayerController->EnableLaser(true);
}

StateType ReloadState::GetType()
{
	return StateType::RELOAD;
}

bool ReloadState::IsReady()
{
    if (mStateTimer.DelayWithoutReset(mStateCooldown) && mPlayerController->CanReload()
        && (App->GetInput()->GetKey(Keys::Keys_R) == KeyState::KEY_DOWN ||
            App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_X) == ButtonState::BUTTON_DOWN))
    {
        return true;
    }
    return false;
}
