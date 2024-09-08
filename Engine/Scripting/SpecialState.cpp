#include "SpecialState.h"
#include "Application.h"

#include "ModuleInput.h"
#include "Keys.h"

#include "PlayerController.h"
#include "Weapon.h"


SpecialState::SpecialState(PlayerController* player, float cooldown) : State(player, cooldown)
{
}

SpecialState::~SpecialState()
{
}

StateType SpecialState::HandleInput()
{
    if (mPlayerController->GetPlayerLowerState()->GetType() == StateType::DASH) return StateType::AIM;

    mSpecialAttackTimer += App->GetDt();
    if (mSpecialAttackTimer < mSpecialWeapon->GetAttackDuration())
    {
		mPlayerController->SetSpineAnimation("tSpecial", 0.3f);
        return StateType::SPECIAL;
    }

    return StateType::AIM;
}

void SpecialState::Update()
{
	mSpecialWeapon->Attack(mSpecialAttackTimer);
}

void SpecialState::Enter()
{
	mSpecialAttackTimer = 0.0f;
	
	mSpecialWeapon = mPlayerController->GetSpecialWeapon();
	if (mSpecialWeapon)
	{
		mPlayerController->SetSpineAnimation("tSpecial", 0.1f);
		mSpecialWeapon->Enter();
	}	
}

void SpecialState::Exit()
{
	mSpecialWeapon->Exit();
	mSpecialWeapon = nullptr;
}

StateType SpecialState::GetType()
{
	return StateType::SPECIAL;
}

bool SpecialState::IsReady()
{

	bool timerReady = false;

	if (mStateTimer.DelayWithoutReset(mStateCooldown))
	{
		timerReady = true;
	}

	float specialPressedCoolDown = (mPlayerController->GetEnergyType() == EnergyType::RED) ? mRedSpecialAttackPressedCoolDown : mBlueSpecialAttackPressedCoolDown;


	if (App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_DOWN ||
		App->GetInput()->GetGameControllerTrigger(LEFT_TRIGGER) == ButtonState::BUTTON_DOWN)
	{
		mPressedSpecialAttackTimer.Reset();
		if (timerReady && mPlayerController->GetEnergyType() != EnergyType::NONE)
		{
				mStateTimer.Reset();
				return true;
		}
	}
	else if ((App->GetInput()->GetMouseKey(MouseKey::BUTTON_RIGHT) == KeyState::KEY_REPEAT ||
		App->GetInput()->GetGameControllerTrigger(LEFT_TRIGGER) == ButtonState::BUTTON_REPEAT) &&
		mPressedSpecialAttackTimer.DelayWithoutReset(specialPressedCoolDown))
	{
		if (mPlayerController->GetWeapon()->GetType() == Weapon::WeaponType::RANGE && mPlayerController->GetEnergyType() != EnergyType::NONE)
		{
			mPressedSpecialAttackTimer.Reset();
			return true;
		}
	}

	//if (mStateTimer.DelayWithoutReset(mStateCooldown) && mPlayerController->GetEnergyType() != EnergyType::NONE) return true;
	return false;
}
