#include "AimState.h"

#include "Application.h"
#include "ModuleInput.h"
#include "Keys.h"
#include "PlayerController.h"
#include "Weapon.h"
#include "GrenadeState.h"
#include "AttackState.h"
#include "SpecialState.h"
#include "SwitchState.h"
#include "ReloadState.h"
#include "UltimateState.h"

AimState::AimState(PlayerController* player, float cooldown) : State(player, cooldown)
{
}

AimState::~AimState()
{
}

StateType AimState::HandleInput()
{
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::DASH;
    if (mPlayerController->GetWeapon()->GetCurrentAmmo()== 0) return StateType::RELOAD;
    if (mPlayerController->GetGrenadeState()->IsReady())
    {
        return StateType::GRENADE;
    }

    if (mPlayerController->GetEnergyType()!=EnergyType::NONE)
    {
        if (mPlayerController->GetSpecialState()->IsReady())
        {
            return StateType::SPECIAL;
        }
    }
    else 
    {
        if (mPlayerController->GetAttackState()->IsReady())
        {
            return StateType::ATTACK;
        }
    }

    //if (mPlayerController->GetSwitchState()->IsReady() &&
    //   (App->GetInput()->GetKey(Keys::Keys_Q) == KeyState::KEY_DOWN ||
    //    App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_Y) == ButtonState::BUTTON_DOWN))
    //{
    //    mPlayerController->GetSwitchState()->ResetCooldown();
    //    return StateType::SWITCH;
    //}

    if (mPlayerController->GetReloadState()->IsReady())
    {
        mPlayerController->GetReloadState()->ResetCooldown();
        return StateType::RELOAD;
    }

    if (mPlayerController->GetUltimateState()->IsReady())
    {
        mPlayerController->GetUltimateState()->ResetCooldown();
        return StateType::ULTIMATE_CHARGE;
    }

	return StateType::AIM;
}

void AimState::Update()
{
    if (!mIsIdle)
    {
        mIdleAnimTimer += App->GetDt();
        if (mIdleAnimTimer >= mWaitTimeForIdle && mPlayerController->GetPlayerLowerState()->GetType() == StateType::IDLE)
        {
            mPlayerController->SetSpineAnimation("tIdleRanged", 0.2f);
            mIsIdle = true;
        }
    }
    else
    {
        mIdleAnimTimer = 0.0f;
        if (mPlayerController->GetPlayerLowerState()->GetType() != StateType::IDLE)
        {
            mIsIdle = false;
            mPlayerController->SetSpineAnimation("tAim", 0.2f);
        }
    }
    

}

void AimState::Enter()
{
    //It can't be done when entering because in that case the others animations would be one frame
    //Maybe with buffers
    /*
    if(mPlayerController->GetPlayerUpperState()->GetType() != StateType::ATTACK)
    {
        if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE)
            mPlayerController->SetSpineAnimation("tIdleRanged", 0.3f);
        else
            mPlayerController->SetSpineAnimation("tIdleMelee", 0.3f);
    }
    */
    mIdleAnimTimer = 0.0f;
    
}

void AimState::Exit()
{
}

StateType AimState::GetType()
{
    return StateType::AIM;
}