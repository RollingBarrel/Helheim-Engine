#include "SpecialState.h"
#include "Application.h"
#include "PlayerController.h"
#include "Weapon.h"

SpecialState::SpecialState(PlayerController* player) : State(player)
{
}

SpecialState::~SpecialState()
{
}

StateType SpecialState::HandleInput()
{
	return StateType::AIM;
}

void SpecialState::Update()
{
	mSpecialWeapon->Attack();
}

void SpecialState::Enter()
{
	mSpecialWeapon = mPlayerController->GetSpecialWeapon();
	mSpecialWeapon->Enter();
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
