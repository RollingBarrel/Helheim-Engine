#pragma once
#include "Weapon.h"

class TrailComponent;

class MeleeWeapon : public Weapon
{
public:
	MeleeWeapon();
	~MeleeWeapon();

	void IncreaseComboStep();
	void MeleeHit();

	float GetAttackTime() override;

protected:
	 int mComboStep = 0;
	 int mNextComboStep = 0;

	 float mComboMilestone1 = 1.0f;
	 float mComboMilestone2 = 2.0f;
	 float mComboDuration = 3.0f;
};

