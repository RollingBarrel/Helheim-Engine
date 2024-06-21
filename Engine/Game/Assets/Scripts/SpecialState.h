#pragma once
#include "State.h"

class Weapon;

class SpecialState : public State
{
public:
	explicit SpecialState(PlayerController* player);
	~SpecialState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

private:
	Weapon* mSpecialWeapon = nullptr;
	float mSpecialAttackTimer = 0.0f;
};