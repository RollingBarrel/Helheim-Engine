#pragma once
#include "State.h"

class Weapon;

class SpecialState : public State
{
public:
	explicit SpecialState(PlayerController* player, float cooldown);
	~SpecialState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;

	bool IsReady() override;

private:
	Weapon* mSpecialWeapon = nullptr;
	float mSpecialAttackTimer = 0.0f;

	TimerScript mPressedSpecialAttackTimer;
	float mRedSpecialAttackPressedCoolDown = 1.0f;
	float mBlueSpecialAttackPressedCoolDown = 0.6f;

};