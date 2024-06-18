#pragma once
#include "State.h"

class Weapon;

class AttackState : public State
{
public:
	explicit AttackState(PlayerController* player);
	~AttackState();

	StateType HandleInput() override;
	void Update() override;

	void Enter() override;
	void Exit() override;

	StateType GetType() override;


private:
	void PlayAudio() override;

	Weapon* mWeapon = nullptr;
	float mAttackTimer = 0.0f;
};