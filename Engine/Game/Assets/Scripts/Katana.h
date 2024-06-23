#pragma once
#include "MeleeWeapon.h"

class TrailComponent;

class Katana : public MeleeWeapon
{
public:
	Katana(BoxColliderComponent* collider, TrailComponent* trail);
	~Katana();

	void Enter() override;
	void Attack(float time) override;
	void Exit() override;

private:

	TrailComponent* mTrail = nullptr;
};
