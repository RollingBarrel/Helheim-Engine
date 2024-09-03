#pragma once
class PlayerStats
{
public:
	float GetMaxHealth() { return mMaxHealth; }
	float GetSpeed() { return mSpeed; }
	float GetDamageModifier() { return mDamageModifier; }
	int GetDialogIndex() { return mDialogIndex; }

	void SetMaxHealth(float maxHealth) { mMaxHealth = maxHealth; }
	void SetSpeed(float speed) { mSpeed = speed; }
	void SetDamageModifier(float damageModifier) { mDamageModifier = damageModifier; }
	void SetDialogIndex(int index) { mDialogIndex = index; }

private:
	float mMaxHealth = 100.0f;
	float mSpeed = 5.5f;
	float mDamageModifier = 1.0f;
	int mDialogIndex = 0;
};

