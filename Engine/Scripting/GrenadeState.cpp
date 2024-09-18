#include "GrenadeState.h"
#include "Application.h"
#include "ModuleInput.h"

#include "GameManager.h"
#include "HudController.h"
#include "PlayerController.h"

#include "AttackState.h"
#include "SpecialState.h"
#include "ReloadState.h"

#include "Keys.h"


StateType GrenadeState::HandleInput()
{
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::DASH;

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

    if (mPlayerController->GetReloadState()->IsReady())
    {
        mPlayerController->GetReloadState()->ResetCooldown();
        return StateType::RELOAD;
    }

    if (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_UP ||
        App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_UP)
    {
        mPlayerController->GetGrenadeState()->ResetCooldown();
        mThrowGrenade = true;
        return StateType::AIM;
    }

    return StateType::GRENADE;
}

void GrenadeState::Update()
{
    mPlayerController->GrenadeAim();
}

void GrenadeState::Enter()
{
    mThrowGrenade = false;

    mPlayerController->EnableGrenadeAim(true);
    mPlayerController->SetSpineAnimation("tGrenade", 0.3f);
}

void GrenadeState::Exit()
{
    mPlayerController->EnableGrenadeAim(false);
    
    if (mThrowGrenade) 
    {
        mPlayerController->ThrowGrenade();
        if (GameManager::GetInstance()->GetHud())
        {
            GameManager::GetInstance()->GetHud()->SetGrenadeCooldown(mPlayerController->GetGrenadeCooldown());
        }
    }
}

bool GrenadeState::IsReady()
{
    if (mStateTimer.DelayWithoutReset(mStateCooldown)
        && mPlayerController->IsGrenadeUnlocked()
        && (App->GetInput()->GetKey(Keys::Keys_E) == KeyState::KEY_DOWN ||
            App->GetInput()->GetGameControllerButton(ControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) == ButtonState::BUTTON_DOWN))
    {
        return true;
    }
        return false;
}

StateType GrenadeState::GetType()
{
    return StateType::GRENADE;
}