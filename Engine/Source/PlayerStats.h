#pragma once
class PlayerStats
{
public:
	float GetMaxHealth() { return mMaxHealth; }
	float GetSpeed() { return mSpeed; }
	float GetDamageModifier() { return mDamageModifier; }

	void SetMaxHealth(float maxHealth) { mMaxHealth = maxHealth; }
	void SetSpeed(float speed) { mSpeed = speed; }
	void SetDamageModifier(float damageModifier) { mDamageModifier = damageModifier; }

private:
	float mMaxHealth = 100.0f;
	float mSpeed = 5.5f;
	float mDamageModifier = 1.0f;
};

