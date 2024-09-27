#pragma once
#include "Globals.h"

class ENGINE_API PlayerStats
{
public:

	float GetMaxHealth() { return mMaxHealth; }
	float GetSpeed() { return mSpeed; }
	float GetDamageModifier() { return mDamageModifier; }
	bool GetGodMode() { return mGodMode; };
	int GetDialogIndex() { return mDialogIndex; }

	void SetMaxHealth(float maxHealth) { mMaxHealth = maxHealth; }
	void SetSpeed(float speed) { mSpeed = speed; }
	void SetDamageModifier(float damageModifier) { mDamageModifier = damageModifier; }
	void SetDialogIndex(int index) { mDialogIndex = index; }
	void SetGodMode(bool godMode) { mGodMode = godMode; };
	void SetInitLevelStats();

	void TryAgainStats();

	void ResetStats();

private:
	float mMaxHealth = 100.0f;
	float mSpeed = 5.5f;
	float mDamageModifier = 1.0f;
	int mDialogIndex = 0;
	bool mGodMode = false;

	float mIniHealth = 0.0f;
	float mIniSpeed = 0.0f;
	float mIniDamage = 0.0f;
	int mIniDialogIdx = 0;
};

